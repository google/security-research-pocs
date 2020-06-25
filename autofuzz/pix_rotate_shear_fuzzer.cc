// The fuzzer takes as input a buffer of bytes. The buffer is read in as:
// <angle>, <x_center>, <y_center>, and the remaining bytes will be read
// in as a <pix>. The image is then rotated by angle around the center. All
// inputs should not result in undefined behavior.
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

#include "allheaders.h"

// Set to true only for debugging; always false for production
static const bool DebugOutput = false;

namespace {

// Reject mallocs larger than 200MiB.
static void* FuzzingMalloc(size_t size) {
  constexpr size_t kMaxFuzzingAllocSize = 209715200;  // 200 MiB
  if (size >= kMaxFuzzingAllocSize) {
    return nullptr;
  }
  return malloc(size);
}

// Initialize leptonica to not allocate more memory than a fixed amount.
bool InitializeLeptonica() {
  setPixMemoryManager(&FuzzingMalloc, &free);
  return true;
}

// Initializes leptonica exactly once.
void InitializeLeptonicaOnce() {
  static bool initialized = InitializeLeptonica();
  (void)initialized;  // Suppress the unused variable warning.
}

// Reads the front bytes of a data buffer containing `size` bytes as an int16_t,
// and advances the buffer forward [if there is sufficient capacity]. If there
// is insufficient capacity, this returns 0 and does not modify size or data.
int16_t ReadInt16(const uint8_t** data, size_t* size) {
  int16_t result = 0;
  if (*size >= sizeof(result)) {
    memcpy(&result, *data, sizeof(result));
    *data += sizeof(result);
    *size -= sizeof(result);
  }
  return result;
}

// Return a pair {success, prod} where prod is the product of values if the
// product of values fits in an int32_t. Overwise, success is false.
std::pair<bool, int32_t> SafeProd(const std::vector<int32_t>& values) {
  int64_t prod = 1;
  for (int32_t x : values) {
    // invariant: INT32_MIN <= prod <= INT32_MAX
    prod *= x;
    if (!(INT32_MIN <= prod && prod <= INT32_MAX)) {
      return {false, 0};
    }
  }
  return {true, prod};
}

// Returns true if the header is reasonable to fuzz.
int32_t ValidateHeader(const uint8_t* data, size_t size) {
  // The format checker requires at least 12 bytes.
  if (size < 12) return false;

  int format, width, height, bps, spp;
  pixReadHeaderMem(data, size, &format, &width, &height, &bps, &spp, nullptr);

  // Check for pnm format; this can cause timeouts.
  if (format == IFF_PNM) return false;

  constexpr int32_t kMaxRasterSize = 75 * (1 << 20);  // 75 MiB
  // Check that the estimated raster size is less than kMaxRasterSize.
  spp = (spp < 3) ? spp : 4;
  const std::pair<bool, int32_t> product = SafeProd({width, height, spp, bps});
  if (!product.first) {
    return false;
  }
  const int32_t estimated_raster_size = product.second / 8;
  return estimated_raster_size < kMaxRasterSize;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  InitializeLeptonicaOnce();

  const int16_t angle = ReadInt16(&data, &size);
  const int16_t x_center = ReadInt16(&data, &size);
  const int16_t y_center = ReadInt16(&data, &size);

  if (!ValidateHeader(data, size)) {
    return EXIT_SUCCESS;
  }

  Pix* pix = pixReadMem(reinterpret_cast<const unsigned char*>(data), size);
  if (pix == nullptr) {
    return EXIT_SUCCESS;
  }

  // Never in production
  if (DebugOutput) {
    L_INFO("w = %d, h = %d, d = %d\n", "fuzzer", pixGetWidth(pix),
           pixGetHeight(pix), pixGetDepth(pix));
  }

  constexpr float deg2rad = M_PI / 180.;
  Pix* pix_rotated = pixRotateShear(pix, x_center, y_center, deg2rad * angle,
                                    L_BRING_IN_WHITE);
  if (pix_rotated) {
    pixDestroy(&pix_rotated);
  }

  pixDestroy(&pix);
  return EXIT_SUCCESS;
}
