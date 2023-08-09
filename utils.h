#include <assert.h>
#include <stdio.h>
#define check_and_assert(func)                                \
  do {                                                        \
    if (!func) {                                              \
      printf("ERROR: %s\n", dlerror());  \
      assert(func);                                           \
    }                                                         \
  } while (0);

#define BINARYSIZE 0x1000

#define RTL2DIFF true
#define DIFF2RTL false
typedef uint64_t word_t;
struct diff_context_t {
  word_t gpr[32];
  word_t fpr[32];
  word_t pc;
  word_t mstatus;
  word_t mcause;
  word_t mepc;
  word_t sstatus;
  word_t scause;
  word_t sepc;
  word_t satp;
  word_t mip;
  word_t mie;
  word_t mscratch;
  word_t sscratch;
  word_t mideleg;
  word_t medeleg;
  word_t mtval;
  word_t stval;
  word_t mtvec;
  word_t stvec;
  word_t priv;
  word_t debugMode;
  word_t dcsr;
  word_t dpc;
  word_t dscratch0;
  word_t dscratch1;
};