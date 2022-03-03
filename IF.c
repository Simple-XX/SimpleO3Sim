#include "IF.h"
#include "interface.h"
#include "ram.h"

struct if_to_id if_to_id_sig[2];
extern struct id_to_if id_to_if_sig[2];

uint32_t fetch_pc = RESET_VEC;

void IF_step() {
    // provide instruction fetch signal
    iram_req.addr = fetch_pc;
    iram_req.wr = false;
    iram_req.size = FETCH_WIDTH; // fetch 4 instrs at a time
    iram_req.wdata = 0; // dont care

    if (id_to_if_sig[0].allow_in) {
        // we are allowed to load instructions
        iram_req.valid = true;
        iram_step();
        if_to_id_sig[1].valid = true;
        if_to_id_sig[1].instr_size = id_to_if_sig[0].instr_allow_size > 4 ? 4 :
                                         id_to_if_sig[0].instr_allow_size;
        for (int i = 0; i < FETCH_WIDTH; ++i) {
            if_to_id_sig[1].instr[i] = iram_resp.rdata[i];
        }
    } else {
        if_to_id_sig[1].valid = false;
    }
}