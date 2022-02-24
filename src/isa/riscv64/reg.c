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
  if (cmd == NULL){
    printf("Reg: ");
    for (int i = 0; i < 32; i++){
      printf("%s: %lu\t",regs[i], cpu.gpr[i]);
      if(i % 8 == 0)
        printf("\n");
    }
    printf("\n");
    return;
  }

  if(strcmp(cmd, "zero") == 0){
    Log("Display $0");
    printf("Reg $0: %lu\n",cpu.gpr[gpr_index]);
    return;
  }
  
  for (;gpr_index < 32; gpr_index++){
    if(strcmp(cmd, regs[gpr_index]) == 0){
      printf("Reg %s: %lu\n", regs[gpr_index], cpu.gpr[gpr_index]);
      return;
    }
  }
  
  if(*cmd == 'x'){
    cmd++;
    int index = atoi(cmd);
    printf("Reg %s: %lu\n", regs[index], cpu.gpr[index]);
  }
  return;
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
