#include "http-parser/http_parser.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  http_parser_url hpu;
  http_parser_parse_url((const char*)data, size, 0, &hpu);
  return 0;
}

