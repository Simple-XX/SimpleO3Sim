#ifndef IF_H
#define IF_H
#include "interface.h"
#include "common.h"
extern struct ram_req iram_req;
extern struct ram_resp iram_resp;
void IF_step();

#define RESET_VEC 0x80000000U
#define FETCH_WIDTH 4
#endif