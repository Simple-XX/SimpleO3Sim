#ifndef CLINT_H

#include "interface.h"

#define CLINT_BASE 0X2000000
#define CLINT_SIZE 0xc000
#define MSIP_OFF 0x0
#define MTIME_OFF 0xBFF8
#define MTIMECMP_OFF 4000

void clint_step();

#endif // CLINT_H