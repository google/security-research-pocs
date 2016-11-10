#include <inttypes.h>
#include <stdio.h>

#include "unicorn/unicorn.h"

#define ADDRESS 0x10000

struct CleanUpHandle {
  uc_engine *handle;
  ~CleanUpHandle() { uc_close(handle); }
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* code_data,
                                      size_t code_size) {
  CleanUpHandle H;
  uc_err err;
  int ecx = 0x12345678;

  
  err = uc_open(UC_ARCH_X86, UC_MODE_32, &H.handle);
  if (err != UC_ERR_OK) {
    return 0;
  }

  
  err = uc_mem_map(H.handle, ADDRESS, 0x1000, UC_PROT_ALL);
  if (err != UC_ERR_OK) {
    return 0;
  }

  err = uc_mem_write(H.handle, ADDRESS, code_data, code_size);
  if (err != UC_ERR_OK) {
    return 0;
  }

  
  err = uc_reg_write(H.handle, UC_X86_REG_ECX, &ecx);
  if (err != UC_ERR_OK) {
    return 0;
  }

  err = uc_emu_start(H.handle, ADDRESS, ADDRESS + code_size, 0, 1);
  if (err != UC_ERR_OK) {
    return 0;
  }

  
  err = uc_reg_read(H.handle, UC_X86_REG_ECX, &ecx);
  if (err != UC_ERR_OK) {
    return 0;
  }

  return 0;
}
