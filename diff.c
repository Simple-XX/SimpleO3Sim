#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "utils.h"
#include "common.h"

void (*spike_memcpy)(uint64_t nemu_addr, void *dut_buf, size_t n, bool direction) = NULL;
void (*regcpy)(void *dut, bool direction) = NULL;
void (*csrcpy)(void *dut, bool direction) = NULL;
void (*uarchstatus_cpy)(void *dut, bool direction) = NULL;
int (*store_commit)(uint64_t *saddr, uint64_t *sdata, uint8_t *smask) = NULL;
void (*spike_exec)(uint64_t n) = NULL;
uint64_t (*guided_exec)(void *disambiguate_para) = NULL;
uint64_t (*update_config)(void *config) = NULL;
void (*raise_intr)(uint64_t no) = NULL;
void (*isa_reg_display)() = NULL;
void (*debug_mem_sync)(uint64_t addr, void *bytes, size_t size) = NULL;

uint8_t binary_buf[1000000];



int init_diff() {
    // load dynamic library from riscv64-spike-so
    void *handle = dlopen("./riscv64-spike-so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }
    // load functions from dynamic library
    void (*init)(void) = dlsym(handle, "init");
    void (*run)(void) = dlsym(handle, "run");
    void (*fini)(void) = dlsym(handle, "fini");

    void (*spike_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
    check_and_assert(spike_init);

    spike_memcpy = (void (*)(uint64_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
    check_and_assert(spike_memcpy);

    regcpy = (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
    check_and_assert(regcpy);

    spike_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
    check_and_assert(spike_exec);

    raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
    check_and_assert(raise_intr);

    isa_reg_display = (void (*)(void))dlsym(handle, "isa_reg_display");
    check_and_assert(isa_reg_display);

    spike_init(0);

    printf("init finished\n");

    // load bianry file to binary buf
    FILE *fp = fopen(ram_file, "r");
    if (!fp) {
        printf("open file failed\n");
        return 1;
    }
    fread(&binary_buf, 1, BINARYSIZE, fp);
    fclose(fp);
    printf("read file finished\n");

    spike_memcpy(0x80000000, &binary_buf, BINARYSIZE, RTL2DIFF);
    printf("memcpy finished\n");

    //for (int i = 0; i < 5; i++) {
    //    spike_exec(1);
    //    isa_reg_display();
    //}

    //
    //regcpy((void *)&regs, DIFF2RTL);
    // print gprs
    //for (int i = 0; i < 32; i++) {
    //    printf("x%d: 0x%llx\n", i, regs.gpr[i]);
    //}
    return 0;
}