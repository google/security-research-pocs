#include <cstdint>
#include <memory>

#include "libopusfile/include/opusfile.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  OggOpusFile *opus_file = op_open_memory(data, size, nullptr);
  if (opus_file == nullptr) {
    return 0;
  }

  auto pcm = std::make_unique<float[]>(size);
  op_read_float(opus_file, pcm.get(), size, nullptr);

  op_free(opus_file);
  return 0;
}
