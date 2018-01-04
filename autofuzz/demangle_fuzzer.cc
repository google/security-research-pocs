#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "demangle.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  char *name = new char[size + 1];
  memcpy(name, data, size);
  name[size] = '\0';  // NUL-terminate
  char *demangled = cplus_demangle(name, DMGL_AUTO);
  if (demangled) free(demangled);
  delete[] name;
  return 0;
}
