#include <stddef.h>
#include <stdint.h>

#include "library/flif_dec.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FLIF_DECODER *decoder = flif_create_decoder();
  flif_decoder_decode_memory(decoder, data, size);
  flif_destroy_decoder(decoder);
  return 0;  // other return values are reserved for future use
}
