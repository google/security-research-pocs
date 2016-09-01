#include <inttypes.h>
#include <stdio.h>

#include <string>

#include "capstone/capstone.h"

struct platform {
  cs_arch arch;
  cs_mode mode;
  std::string comment;
};

// Note: changing this struct will modify reproducibility of previous crashes.
// Taken from test_detail.c.
struct platform platforms[] = {
    {CS_ARCH_X86, CS_MODE_32, "X86 32"},
    {CS_ARCH_X86, CS_MODE_64, "X86 64"},
    {CS_ARCH_ARM, CS_MODE_ARM, "ARM"},
    {CS_ARCH_ARM, CS_MODE_THUMB, "THUMB-2"},
    {CS_ARCH_ARM, CS_MODE_ARM, "ARM: Cortex-A15 + NEON"},
    {CS_ARCH_ARM, CS_MODE_THUMB, "THUMB"},
    {CS_ARCH_ARM, (cs_mode)(CS_MODE_THUMB + CS_MODE_MCLASS), "Thumb-MClass"},
    {CS_ARCH_ARM, (cs_mode)(CS_MODE_ARM + CS_MODE_V8), "Arm-V8"},
    {CS_ARCH_MIPS, (cs_mode)(CS_MODE_MIPS32 + CS_MODE_BIG_ENDIAN),
     "MIPS-32 (Big-endian)"},
    {CS_ARCH_MIPS, (cs_mode)(CS_MODE_MIPS64 + CS_MODE_LITTLE_ENDIAN),
     "MIPS-64-EL (Little-endian)"},
    {CS_ARCH_MIPS,
     (cs_mode)(CS_MODE_MIPS32R6 + CS_MODE_MICRO + CS_MODE_BIG_ENDIAN),
     "MIPS-32R6 | Micro (Big-endian)"},
    {CS_ARCH_MIPS, (cs_mode)(CS_MODE_MIPS32R6 + CS_MODE_BIG_ENDIAN),
     "MIPS-32R6 (Big-endian)"},
    {CS_ARCH_ARM64, CS_MODE_ARM, "ARM-64"},
    {CS_ARCH_PPC, CS_MODE_BIG_ENDIAN, "PPC-64"},
    {CS_ARCH_SPARC, CS_MODE_BIG_ENDIAN, "Sparc"},
    {CS_ARCH_SPARC, (cs_mode)(CS_MODE_BIG_ENDIAN + CS_MODE_V9), "SparcV9"},
    {CS_ARCH_SYSZ, (cs_mode)0, "SystemZ"},
    {CS_ARCH_XCORE, (cs_mode)0, "XCore"},
};

int platforms_len = sizeof(platforms) / sizeof(platforms[0]);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t data_len) {
  if (data_len < 3) {
    return 0;
  }

  csh handle;
  cs_insn *all_insn;
  cs_err err;

  int selected_platform = (int)data[0] % platforms_len;

  const uint8_t *buf_ptr = data + 1;
  size_t buf_ptr_size = data_len - 1;

#if 0
  printf("Platform: %s (0x%.2x of 0x%.2x)\n",
         platforms[selected_platform].comment.c_str(), selected_platform,
         platforms_len);
#endif

  err = cs_open(platforms[selected_platform].arch,
                platforms[selected_platform].mode, &handle);
  if (err) {
    return 0;
  }

  cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

  uint64_t address = 0x1000;
  size_t count =
      cs_disasm(handle, buf_ptr, buf_ptr_size, address, 0, &all_insn);

  if (!count) {
    cs_close(&handle);
    return 0;
  }

#if 0
  size_t j;
  int n;

  for (j = 0; j < count; j++) {
    cs_insn *i = &(all_insn[j]);
    printf("0x%" PRIx64 ": %s %s // insn-ID: %u, insn-mnem: %s\n", i->address,
           i->mnemonic, i->op_str, i->id, cs_insn_name(handle, i->id));

    cs_detail *detail = i->detail;

    if (detail->regs_read_count > 0) {
      printf("Implicit registers read: \n");
      for (n = 0; n < detail->regs_read_count; n++) {
        printf("%s \n", cs_reg_name(handle, detail->regs_read[n]));
      }
    }

    if (detail->regs_write_count > 0) {
      printf("Implicit registers modified: \n");
      for (n = 0; n < detail->regs_write_count; n++) {
        printf("%s \n", cs_reg_name(handle, detail->regs_write[n]));
      }
    }

    if (detail->groups_count > 0) {
      printf("This instruction belongs to groups: \n");
      for (n = 0; n < detail->groups_count; n++) {
        printf("%s \n", cs_group_name(handle, detail->groups[n]));
      }
    }
  }
  printf("0x%" PRIx64 ":\n", all_insn[j - 1].address + all_insn[j - 1].size);
#endif

  cs_free(all_insn, count);

  cs_close(&handle);

  return 0;
}
