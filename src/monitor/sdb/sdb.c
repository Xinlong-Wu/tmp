#include <isa.h>
#include <memory/paddr.h>
#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_info(char *args) {
  Log("cmd_info get arg %s", args);
  char *cmd = strtok(args, " ");
  args = cmd + strlen(cmd) +1;

  if (strcmp(cmd,"r") == 0){
    isa_reg_display(args);
  }
  else if (strcmp(cmd, "w") == 0){
    Log("TBD: Show WatchPoint %s",args);
  }

  return 0;
}

extern word_t vaddr_read(vaddr_t addr, int len);

static int cmd_x(char *args) {

  int len = 0;
  paddr_t addr = 0;
  if(args)
    sscanf(args,"%d %x",&len,&addr);

  if(len != 0 && addr!=0){
    int onceLength = sizeof(word_t) < len ? sizeof(word_t) : (len>>1)<<1;
    if (onceLength == 0)
      onceLength = 1;
    
    if(!in_pmem(addr))
      addr += CONFIG_MBASE;
    if(in_pmem(addr)){
      int printCount = 0;
      printf("0x%x:\t", addr);
      for(int i = len;i > 0; i-=onceLength){
        word_t data = vaddr_read(addr, onceLength);
        printf("0x%016lx\t",data);
        if (printCount++ % 4 == 0)
          printf("\n\t");
        addr+=onceLength;
        while (onceLength > i)
          onceLength/=2;
      }
    }
    else
      printf("%s\n", ASNI_FMT(str(Error: valid mem address.), ASNI_FG_RED));
    return 0;
  }
  printf("%s\n", ASNI_FMT(str(Error: valid mem address.), ASNI_FG_RED));
  return 0;
}

static int cmd_si(char *args){
  int n = 1;
  if(args)
    sscanf(args,"%x",&n);
  
  Decode s;
  s.pc = cpu.pc;
  s.snpc = cpu.pc;
  while (n--){
    isa_exec_once(&s);
    cpu.pc = s.dnpc;
  }
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si", "si [N], step in the program", cmd_si},
  {"info", "info [r|w], out put the info of Regesiter or WatchPoint", cmd_info},
  {"x", "x [N] [EXPR] , out put N Bite data from value of EXPR by sixteen format", cmd_x},
  // {"p", "p [EXPR], out put the value of EXPR"},
  // {"w", "w [EXPR], set WatchPoint stop the program if the value of EXPR has changed"},
  // {"d", "d [N], delete WatchPoint witch id is N"},
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  Log("SDB Mod");
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
