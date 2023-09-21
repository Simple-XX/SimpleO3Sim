#include "IF.h"
#include "interface.h"
#include "ram.h"

struct if_to_id if_to_id_sig[2];
extern struct id_to_if id_to_if_sig[2];

extern struct jmp_redirectInfo jmp_to_is_sig[2];

uint64_t fetch_pc = RESET_VEC;

void IF_step() {
    if (jmp_to_is_sig[0].redirect_valid) {
        fetch_pc = jmp_to_is_sig[0].redirect_pc;
    }

    // provide instruction fetch signal
    iram_req.addr = fetch_pc - 0x80000000;
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
        #ifdef DEBUG
        printf("IF stage: fetched %d instructions\n", if_to_id_sig[1].instr_size);
        printf("IF stage: instructions: ");
        #endif // DEBUG
        
        for (int i = 0; i < FETCH_WIDTH; ++i) {
            if_to_id_sig[1].instr[i] = iram_resp.rdata[i];
            #ifdef DEBUG
            printf("0x%8x ", if_to_id_sig[1].instr[i]);
            #endif // DEBUG
        }
        #ifdef DEBUG
        printf("\n");
        #endif // DEBUG
        if_to_id_sig[1].fetch_pc = fetch_pc;
        fetch_pc += id_to_if_sig[0].instr_allow_size * 4;
    } else {
        if_to_id_sig[1].valid = false;
    }
}