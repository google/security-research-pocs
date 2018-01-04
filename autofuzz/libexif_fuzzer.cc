#include <stddef.h>
#include <libexif/exif-data.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  auto image = exif_data_new_from_data(data, size);
  if (image) {
    exif_data_get_mnote_data(image);
    exif_data_fix(image);
    exif_data_unref(image);
  }
  return 0;
}
