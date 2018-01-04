#include <string>
#include "tinyxml.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  std::string s(reinterpret_cast<const char*>(data), size);
  TiXmlDocument doc("fuzz.xml");
  doc.Parse(s.c_str());
  return 0;
}
