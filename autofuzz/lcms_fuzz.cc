#include <stdint.h>
#include <string>
#include "lcms2.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 2) {
    return 0;
  }

  size_t mid = size / 2;

  cmsHPROFILE hInProfile, hOutProfile;
  cmsHTRANSFORM hTransform;

  hInProfile = cmsOpenProfileFromMem(data, mid);
  hOutProfile = cmsOpenProfileFromMem(data + mid, size - mid);
  hTransform = cmsCreateTransform(hInProfile, TYPE_BGR_8, hOutProfile,
                                  TYPE_BGR_8, INTENT_PERCEPTUAL, 0);
  cmsCloseProfile(hInProfile);
  cmsCloseProfile(hOutProfile);

  if (hTransform) {
    cmsDeleteTransform(hTransform);
  }
  return 0;
}
