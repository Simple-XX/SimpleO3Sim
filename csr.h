#ifndef CSR_H
#define CSR_H

#include "interface.h"
#include "common.h"

void csr_step();
uint64_t * get_csr(uint64_t addr);

#define CSR_MSTATUS 0x300
#define CSR_MISA 0x301
#define CSR_MIE 0x304
#define CSR_MTVEC 0x305
#define CSR_MSCRATCH 0x340
#define CSR_MEPC 0x341
#define CSR_MCAUSE 0x342
#define CSR_MTVAL 0x343
#define CSR_MIP 0x344
#define CSR_MTVAL2 0x34b

#define WMASK_MSTATUS  0x1888
#define WMASK_MISA     0x0
#define WMASK_MIE      0xffffffff
#define WMASK_MTVEC    0xffffffff
#define WMASK_MSCRATCH 0xffffffff
#define WMASK_MEPC     0xffffffff
#define WMASK_MCAUSE   0xffffffff
#define WMASK_MTVAL    0xffffffff
#define WMASK_MIP      0xffffffff
#define WMASK_MTVAL2   0xffffffff

#endif // CSR_H