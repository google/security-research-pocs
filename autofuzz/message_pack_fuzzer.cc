// MessagePack fuzzing wrapper to help with automated fuzz testing.

#include <stdint.h>
#include <cstdio>

#include <iostream>
#include "json/src/json.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  try {
    nlohmann::json j = nlohmann::json::from_msgpack(data, data + size);
    std::string s = j.dump();
    std::cout << s.c_str() << std::endl;
  } catch (const nlohmann::json::parse_error&) {
    // Do nothing.
  } catch (const nlohmann::detail::type_error&) {
    // Do nothing again.
  }

  return 0;
}
