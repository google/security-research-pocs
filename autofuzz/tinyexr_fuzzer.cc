// Fuzzer for singlepart EXR images.
//
// This implementation is a copy of LoadEXRFromMemory. We can't use
// LoadEXRFromMemory because it assumes the user knows the dimensions of the
// image and can pre-allocate the out_rgba buffer.

#include <stddef.h>
#include <stdint.h>


#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  EXRVersion exr_version;
  EXRImage exr_image;
  EXRHeader exr_header;

  int ret = ParseEXRVersionFromMemory(&exr_version, data, size);
  if (ret != TINYEXR_SUCCESS) {
    return 0;
  }

  InitEXRHeader(&exr_header);
  ret = ParseEXRHeaderFromMemory(&exr_header, &exr_version, data, size, NULL);
  if (ret != TINYEXR_SUCCESS) {
    FreeEXRHeader(&exr_header);
    return 0;
  }

  InitEXRImage(&exr_image);
  ret = LoadEXRImageFromMemory(&exr_image, &exr_header, data, size, NULL);
  if (ret != TINYEXR_SUCCESS) {
    FreeEXRHeader(&exr_header);
    FreeEXRImage(&exr_image);
    return 0;
  }

  FreeEXRHeader(&exr_header);
  FreeEXRImage(&exr_image);

  return 0;
}
