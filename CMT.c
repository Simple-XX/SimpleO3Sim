#include "CMT.h"

extern struct ex_to_cmt ex_to_cmt_sig[2];
struct ex_to_cmt ex_to_cmt_sig[2];

struct cmt_wakeup_info cmt_wakeup_sig[2];

#include "RN.h"

extern uint32_t prf[PRF_SIZE];
extern bool prf_ready[PRF_SIZE];

// commit_head: the index of the instruction that
// waits to be commited

// rob_offset: the current offset of the instruction in
// rob buffer

uint64_t commit_head, rob_offset;

struct commitInfo rob[ROB_SIZE];


/*
 * ROB here is a loop queue
 * 
 */

void CMT_step() {
    // time to commit
    if (ex_to_cmt_sig[0].valid) {
        for (int i = 0; i < ex_to_cmt_sig[0].jmp_size; ++i) {
            rob[rob_offset + ex_to_cmt_sig[0].jmp[i].idx - commit_head] = ex_to_cmt_sig[0].jmp[i];
        }
        for (int i = 0; i < ex_to_cmt_sig[0].alu_size; ++i) {
            rob[rob_offset + ex_to_cmt_sig[0].alu[i].idx - commit_head] = ex_to_cmt_sig[0].alu[i];
        }
        for (int i = 0; i < ex_to_cmt_sig[0].mdu_size; ++i) {
            rob[rob_offset + ex_to_cmt_sig[0].mdu[i].idx - commit_head] = ex_to_cmt_sig[0].mdu[i];
        }
        for (int i = 0; i < ex_to_cmt_sig[0].lsu_size; ++i) {
            rob[rob_offset + ex_to_cmt_sig[0].lsu[i].idx - commit_head] = ex_to_cmt_sig[0].lsu[i];
        }
    }
    int commit_counter = 0;
    while (1) {
        // commit until not valid
        if (rob[rob_offset].slot_valid) {
            // ready to commit
            
            // handle register write
            if (rob[rob_offset].rd_valid) {
                prf[rob[rob_offset].renamed.rd_phy.a] = rob[rob_offset].rd_data;
                prf_ready[rob[rob_offset].renamed.rd_phy.a] = true;
            }
            // handle memory write

            ++commit_counter;
            rob_offset = (rob_offset + 1) % ROB_SIZE;
            ++commit_head;
            if (commit_counter >= COMMIT_SIZE) {
                // reach our limitation
                break;
            }
        } else {
            break;
        }
    }
}