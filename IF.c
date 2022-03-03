#include "IF.h"
#include "interface.h"

struct if_to_id if_to_id_sig[2];
extern struct id_to_if id_to_if_sig[2];

uint32_t fetch_pc = RESET_VEC;

void IF_step() {
    // provide instruction fetch signal
    iram_req.addr = fetch_pc;
    iram_req.wr = false;
    iram_req.size = 4; // fetch 4 instrs at a time
    iram_req.wdata = 0; // dont care

}