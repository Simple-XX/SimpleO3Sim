#include "interface.h"
#include "uart.h"
#include "ram.h"
#include <stdio.h>

void uart_step() {
    // only support write operation
    if (dram_req.valid && dram_req.addr == 0x40000000U) {
        assert(dram_req.wr);
        assert(dram_req.size == 0);
        // printf("%c", dram_req.wdata);
        putc(dram_req.wdata, stdout);
    }
}