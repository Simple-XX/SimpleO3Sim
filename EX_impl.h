#ifndef EX_IMPL_H
#define EX_IMPL_H

#include "interface.h"
#include "common.h"

void ALU_calc(struct decode_info decoded, struct rename_info renamed, int offset);
void LSU_calc(struct decode_info decoded, struct rename_info renamed, int offset); 
void MDU_calc(struct decode_info decoded, struct rename_info renamed, int offset);
void JMP_calc(struct decode_info decoded, struct rename_info renamed, int offset);
void CSR_calc(struct decode_info decoded, struct rename_info renamed, int offset);
#endif