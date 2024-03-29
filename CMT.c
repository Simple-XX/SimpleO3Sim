#include "CMT.h"
#include "utils.h"
#include "diff.h"

extern struct ex_to_cmt ex_to_cmt_sig[2];
struct cmt_to_ex cmt_to_ex_sig[2];

struct cmt_wakeup_info cmt_wakeup_sig[2];

extern struct jmp_redirectInfo jmp_to_is_sig[2];

#ifdef REG_DEBUG
#include "RN.h"
uint64_t cmt_arf[32];

char* reg_name[32] = {"x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

extern uint64_t prf[PRF_SIZE];
#endif // REG_DEBUG

#include "RN.h"

extern uint64_t prf[PRF_SIZE];
extern bool prf_ready[PRF_SIZE];

// commit_head: the index of the instruction that
// waits to be commited

// rob_offset: the current offset of the instruction in
// rob buffer

uint64_t commit_head, rob_offset;

struct commitInfo rob[ROB_SIZE];

struct diff_context_t regs;

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
            printf("alu commit idx %llu\n", ex_to_cmt_sig[0].alu[i].idx % ROB_SIZE);
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
        #ifdef DEBUG
        printf("csr unit wants to commit %d instrs\n", ex_to_cmt_sig[0].csr_size);
        #endif // DEBUG
        //rob_offset += ex_to_cmt_sig[0].lsu_size;
        for (int i = 0; i < ex_to_cmt_sig[0].csr_size; ++i) {
            printf("idx %llu\n", ex_to_cmt_sig[0].csr[i].idx % ROB_SIZE);
            rob[ex_to_cmt_sig[0].csr[i].idx % ROB_SIZE] = ex_to_cmt_sig[0].csr[i];
        }
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
    printf("commit head %llu\n", commit_head);
    while (1) {
        // commit until not valid
        if (rob[commit_head].slot_valid) {
            rob[commit_head].slot_valid = false; // clear after commit
            #ifdef DEBUG
            printf("rob real commit: pc 0x%x instr idx = %llu ard %d rd data 0x%x\n", rob[commit_head].pc, rob[commit_head].idx, rob[commit_head].ard, rob[commit_head].rd_data);
            #endif // DEBUG
            // ready to commit
            
            printf("rd valid %d rd_phy %d\n", rob[commit_head].rd_valid, rob[commit_head].renamed.rd_phy.a);
            // handle register write
            if (rob[commit_head].rd_valid && rob[commit_head].renamed.arch_rd != 0) {
                prf[rob[commit_head].renamed.rd_phy.a] = rob[commit_head].rd_data;
                prf_ready[rob[commit_head].renamed.rd_phy.a] = true;

                #ifdef REG_DEBUG
                if (rob[commit_head].ard != 0) {
                    cmt_arf[rob[commit_head].ard] = rob[commit_head].rd_data;
                    printf("arf reg %d write data 0x%x\n", rob[commit_head].ard, rob[commit_head].rd_data);
                }
                printf("rob phy reg %d write data 0x%x\n", rob[commit_head].renamed.rd_phy.a, rob[commit_head].rd_data);

            }
            #ifdef DIFFTEST
            // difftest check here
            spike_exec(1);
            regcpy((void *)&regs, DIFF2RTL);
            for (int i = 0; i < 32; i++) {
                if (regs.gpr[i] != cmt_arf[i]) {
                    printf("reg %d mismatch: Spike 0x%llx, O3Sim 0x%llx\n", i, regs.gpr[i], cmt_arf[i]);
                    assert(0);
                }
            }
            printf("Difftest pass this cycle\n");
            isa_reg_display();
            #endif // DIFFTEST
            
            #endif // REG_DEBUG
            
            // handle memory write
            if (rob[commit_head].rd_valid && rob[commit_head].renamed.arch_rd != 0) {
                cmt_wakeup_sig[1].committed[commit_counter].recycle_dst = rob[commit_head].renamed.rd_phy.a;
            } else {
                cmt_wakeup_sig[1].committed[commit_counter].recycle_dst = -1;
            }
            if (rob[commit_head].renamed.is_csr) {
                cmt_wakeup_sig[1].committed[commit_counter].is_csr = true;
                cmt_wakeup_sig[1].committed[commit_counter].csr_instr = rob[commit_head].renamed.csr_instr;
            } else {
                cmt_wakeup_sig[1].committed[commit_counter].is_csr = false;
            }
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