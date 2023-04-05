#include "CMT.h"

extern struct ex_to_cmt ex_to_cmt_sig[2];
struct cmt_to_ex cmt_to_ex_sig[2];

struct cmt_wakeup_info cmt_wakeup_sig[2];

extern struct jmp_redirectInfo jmp_to_is_sig[2];

#ifdef REG_DEBUG
#include "RN.h"
uint32_t cmt_arf[32];

char* reg_name[32] = {"x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

extern uint32_t prf[PRF_SIZE];
#endif // REG_DEBUG

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
        #ifdef DEBUG
        printf("CMT: ready to commit:\n");
        #endif // DEBUG

        #ifdef DEBUG
        printf("jmp unit wants to commit %d instrs\n", ex_to_cmt_sig[0].jmp_size);
        #endif // DEBUG
        for (int i = 0; i < ex_to_cmt_sig[0].jmp_size; ++i) {
            rob[ex_to_cmt_sig[0].jmp[i].idx % ROB_SIZE] = ex_to_cmt_sig[0].jmp[i];
        }
        //rob_offset += ex_to_cmt_sig[0].jmp_size;
        #ifdef DEBUG
        printf("alu unit wants to commit %d instrs\n", ex_to_cmt_sig[0].alu_size);
        #endif // DEBUG
        for (int i = 0; i < ex_to_cmt_sig[0].alu_size; ++i) {
            rob[ex_to_cmt_sig[0].alu[i].idx % ROB_SIZE] = ex_to_cmt_sig[0].alu[i];
        }
        //rob_offset += ex_to_cmt_sig[0].alu_size;
        #ifdef DEBUG
        printf("mdu unit wants to commit %d instrs\n", ex_to_cmt_sig[0].mdu_size);
        #endif // DEBUG
        for (int i = 0; i < ex_to_cmt_sig[0].mdu_size; ++i) {
            rob[ex_to_cmt_sig[0].mdu[i].idx % ROB_SIZE] = ex_to_cmt_sig[0].mdu[i];
        }
        //rob_offset += ex_to_cmt_sig[0].mdu_size;
        #ifdef DEBUG
        printf("lsu unit wants to commit %d instrs\n", ex_to_cmt_sig[0].lsu_size);
        #endif // DEBUG
        for (int i = 0; i < ex_to_cmt_sig[0].lsu_size; ++i) {
            rob[ex_to_cmt_sig[0].lsu[i].idx % ROB_SIZE] = ex_to_cmt_sig[0].lsu[i];
        }
        //rob_offset += ex_to_cmt_sig[0].lsu_size;
    }

    #ifdef DEBUG
    if (jmp_to_is_sig[0].redirect_valid) {
        printf("redirect at ROB, instr idx = %llu\n", jmp_to_is_sig[0].instr_idx);
    }
    #endif
    for (int i = 0; i < ROB_SIZE; ++i) {
        if (jmp_to_is_sig[0].redirect_valid && rob[i].slot_valid && jmp_to_is_sig[0].instr_idx < rob[i].idx) {
            rob[i].slot_valid = false;
        }
    }

    int commit_counter = 0;
    while (1) {
        // commit until not valid
        if (rob[commit_head].slot_valid) {
            rob[commit_head].slot_valid = false; // clear after commit
            #ifdef DEBUG
            printf("rob real commit: pc 0x%x instr idx = %llu\n", rob[commit_head].pc, rob[commit_head].idx);
            #endif // DEBUG
            // ready to commit
            
            // handle register write
            if (rob[commit_head].rd_valid && rob[commit_head].renamed.rd_phy.a != 0) {
                prf[rob[commit_head].renamed.rd_phy.a] = rob[commit_head].rd_data;
                prf_ready[rob[commit_head].renamed.rd_phy.a] = true;

                #ifdef REG_DEBUG
                if (rob[commit_head].ard != 0) {
                    cmt_arf[rob[commit_head].ard] = rob[commit_head].rd_data;
                }
                printf("rob phy reg %d write data 0x%x\n", rob[commit_head].renamed.rd_phy.a, rob[commit_head].rd_data);
                
                #endif // REG_DEBUG
            }
            
            // handle memory write
            cmt_wakeup_sig[1].committed[commit_counter].recycle_dst = rob[commit_head].renamed.rd_phy.b;
            ++commit_counter;
            commit_head = (commit_head + 1) % ROB_SIZE;
            if (commit_counter >= COMMIT_SIZE) {
                // reach our limitation
                break;
            }
        } else {
            break;
        }
    }
    cmt_wakeup_sig[1].valid = true;
    cmt_wakeup_sig[1].commit_size = commit_counter;
    
    #ifdef REG_DEBUG
    printf("Reg Dump after commit:\n");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {
            printf("%s: 0x%08x ", reg_name[i * 8 + j], cmt_arf[i * 8 + j]);
        }
        printf("\n");
        
    }
    printf("Phy Reg Dump after commit:\n");
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 8; ++j) {
            printf("phy %2d:0x%08x ", i * 8 + j, prf[i * 8 + j]);
        }
        printf("\n");
    }
    #endif // REG_DEBUG
}