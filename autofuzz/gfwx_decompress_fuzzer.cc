#include <stdint.h>

#include "gfwx/gfwx.h"

static unsigned char buffer[1 << 29];  // 512 MB.

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  GFWX::Header h;

  if (GFWX::decompress(static_cast<uint8_t *>(nullptr), h, data, size,
                       0 /* downsampling */,
                       false /* test */) != GFWX::ResultOk) {
    return 0;
  }

  size_t sz = h.bufferSize();
  // Reject invalid inputs and pathologically large inputs.
  if (sz == 0 || sz > sizeof(buffer)) {
    return 0;
  }

  GFWX::decompress(reinterpret_cast<uint8_t *>(buffer), h, data, size,
                   0 /* downsampling */, false /* test */);

  return 0;
}
