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

  bool isSuccess = false;
  word_t res = isa_reg_str2val(cmd, &isSuccess);

  if(isSuccess)
    printf("Reg %s: %lu\n", cmd, res);
  else
    printf("%s\n", ASNI_FMT(str(Error: wrong register.), ASNI_FG_RED));

  return;
}

word_t isa_reg_str2val(const char *s, bool *success) {

  *success = false;

  if(strcmp(s, "zero") == 0){
    *success = true;
    return cpu.gpr[0];
  }

  if(*s == 'x'){
    int index = atoi(s+1);
    if (index < 32){
      *success = true;
      return cpu.gpr[index];
    }
    return 0;
  }

  for (int i = 0;i < 32; i++){
    if(strcmp(s, regs[i]) == 0){
      *success = true;
      return cpu.gpr[i];
    }
  }
  
  return 0;
}
