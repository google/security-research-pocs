#include <string>

#include "lib/gvc/gvc.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 3) return 0;
  std::string s(reinterpret_cast<const char*>(data), size);
  agseterr(AGMAX);  // Don't print to stderr.
  if (auto a = agmemread(s.c_str())) {
    agclose(a);
  }
  return 0;
}
