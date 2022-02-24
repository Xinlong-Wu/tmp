#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

extern riscv64_CPU_state cpu;

void isa_reg_display(char* reg) {
  int gpr_index = 0;

  Log("isa_reg_display get arg %s", reg);

  char *cmd = strtok(reg, " ");
  if(strcmp(cmd, "zero")){
    Log("Display $0");
    return;
  }
  
  for (;gpr_index < 32; gpr_index++){
    if(strcmp(cmd, regs[gpr_index])){
      Log("Reg %s: %lud", regs[gpr_index], cpu.gpr[gpr_index]);
    }
  }
  
  
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
