#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>  // for free()
#include <string.h>  // for strdup()
#include <string>

#include "inchi_api.h"

char *szOptions = strdup("");

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  char *x =
      strdup(std::string(reinterpret_cast<const char *>(data), size).c_str());

  char inchiKey[29], xtra1[65], xtra2[65];
  GetINCHIKeyFromINCHI(x, 0, 0, inchiKey, xtra1, xtra2);

  inchi_InputINCHI i;
  i.szInChI = x;
  i.szOptions = szOptions;

  inchi_Output o;
  GetINCHIfromINCHI(&i, &o);
  FreeINCHI(&o);

  inchi_OutputStruct os;
  GetStructFromINCHI(&i, &os);
  FreeStructFromINCHI(&os);

  free(x);
  return 0;
}
