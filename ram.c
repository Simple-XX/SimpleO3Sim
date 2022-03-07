#include "ram.h"
#include "common.h"
#include <stdio.h>
#include "log.h"
#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

#define RAM_SIZE 0x1000000 // 10 MB

FILE* ram_file_ptr;

char* ram;
uint32_t *ram_word;

struct ram_req iram_req, dram_req;
struct ram_resp iram_resp, dram_resp;

void init_ram() {
    // init ram if a ram path is provided
    if ((ram_file_ptr = fopen(ram_file, "r")) == NULL) {
        LOG("cannot load ram file");
    }
    int c, load_size = 0;
    ram = (char *)malloc(RAM_SIZE);
    while ((c = fgetc(ram_file_ptr)) != EOF) {
        ram[load_size++] = c;
        assert(load_size < RAM_SIZE);
    }
    ram_word = (uint32_t *)ram;
}

void iram_step() {
    // handle instruction memory
    assert(iram_req.wr == 0);
    if (iram_req.valid && iram_req.size > 1) {
        // does not need to clear rdata for it will be fully overriden
        for (int i = 0; i < (1 << (iram_req.size - 2)); ++i) {
            iram_resp.rdata[i] = ram_word[(iram_req.addr >> 2) + i];
        }
    } else {
        iram_resp.rdata[0] = 0;
        for (int i = (1 << iram_req.size) - 1; i >= 0; --i) {
            iram_resp.rdata[0] = (iram_resp.rdata[0] << 8) | (ram[iram_req.addr + i]);
        }
    }
}

void dram_step() {
    // handle data memory
    if (dram_req.valid) {
        if (dram_req.wr) {
            // cannot write more than 4 bytes in a single op
            assert(dram_req.size < 3);
            assert(!((dram_req.addr & 0x3) && (dram_req.size == 2)));
            assert(!((dram_req.addr & 0x1) && (dram_req.size == 1)));
            uint32_t wdata = dram_req.wdata;
            for (int i = 0; i < (1 << dram_req.size); ++i) {
                ram[dram_req.addr + i] = wdata & 0xff;
                wdata >>= 8;
            }
        } else {
            if (dram_req.size > 1) {
                // does not need to clear rdata for it will be fully overriden
                for (int i = 0; i < (1 << (dram_req.size - 2)); ++i) {
                    dram_resp.rdata[i] = ram_word[(dram_req.addr >> 2) + i];
                }
            } else {
                iram_resp.rdata[0] = 0;
                for (int i = (1 << dram_req.size) - 1; i >= 0; --i) {
                    dram_resp.rdata[0] = (dram_resp.rdata[0] << 8) | (ram[dram_req.addr + i]);
                }
            }
        }
    }
}