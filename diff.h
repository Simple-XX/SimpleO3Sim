#ifndef DIFF_H
#define DIFF_H
void (*spike_memcpy)(uint64_t nemu_addr, void *dut_buf, size_t n, bool direction);
void (*regcpy)(void *dut, bool direction);
void (*csrcpy)(void *dut, bool direction);
void (*uarchstatus_cpy)(void *dut, bool direction);
int (*store_commit)(uint64_t *saddr, uint64_t *sdata, uint8_t *smask);
void (*spike_exec)(uint64_t n);
uint64_t (*guided_exec)(void *disambiguate_para);
uint64_t (*update_config)(void *config);
void (*raise_intr)(uint64_t no);
void (*isa_reg_display)();
void (*debug_mem_sync)(uint64_t addr, void *bytes, size_t size);

int init_diff();
#endif