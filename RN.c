#include "RN.h"

extern struct cmt_wakeup_info cmt_wakeup_sig[2];

extern struct id_to_rn id_to_rn_sig[2];
struct rn_to_id rn_to_id_sig[2];

struct rn_to_is rn_to_is_sig[2];
extern struct is_to_rn is_to_rn_sig[2];

// always assign r0 with prf[0]
uint32_t prf[PRF_SIZE];
bool prf_ready[PRF_SIZE];

// free list act as a stack
int prf_free_list[PRF_SIZE];
int prf_free_list_size = PRF_SIZE - 32;

int arf_prf_map[JMP_DEPTH][32];
int current_jmp = 0;

void RN_init() {
    for (int i = 1; i < PRF_SIZE; ++i) {
        prf[i] = 0xdeadbeef;
    }

    for (int i = 0; i < 32; ++i) {
        prf_ready[i] = true;
    }

    // init prf_free_list
    for (int i = 31; i < PRF_SIZE - 1; ++i) {
        prf_free_list[i] = i + 1;
    }
    // by defauly 0-31 arf maps to 0-31 prf
    for (int i = 0; i < 32; ++i) {
        arf_prf_map[0][i] = i;
    }
    
}

int alloc_src(int src) {
    assert(src >= 0 && src < 32);
    // find the corresponding src regsiter
    if (src == 0) return 0; // alway zero, does not bother rename
    return arf_prf_map[current_jmp][src];
}


struct int_pair alloc_dst(int dst) {
    assert(dst >= 0 && dst < 32);
    struct int_pair ret;
    // find a free reg and map it as new dst register
    if (dst == 0) {
        ret.a = 0;
        ret.b = 0;
        return ret;
    }
    assert(prf_free_list_size > 0);
    int ret_reg = prf_free_list[--prf_free_list_size];
    // we need to remember its old map so that we can recycle
    // this reg when we commit
    int old_reg = arf_prf_map[current_jmp][dst];
    arf_prf_map[current_jmp][dst] = ret_reg;
    
    ret.a = ret_reg;
    ret.b = old_reg;
    prf_ready[ret_reg] = false;
    return ret;
}

void commit_dst(int dst) {
    assert(dst >= 0 && dst < PRF_SIZE);
    // since instruction commited, we are free to recycle its previous map
    prf_free_list[prf_free_list_size++] = dst;
    prf_ready[dst] = true;
}

void RN_step() {
    if (cmt_wakeup_sig[0].valid) {
        for (int i = 0; i < cmt_wakeup_sig[0].commit_size; ++i) {
            commit_dst(cmt_wakeup_sig[0].committed[i].recycle_dst);
            #ifdef DEBUG
            printf("RN: waking up prf %d\n", cmt_wakeup_sig[0].committed[i].recycle_dst);
            #endif // DEBUG
        }
    }
    
    // rename
    if (id_to_rn_sig[0].valid) {
        #ifdef DEBUG
        printf("RN: rename size: %d\n", id_to_rn_sig[0].decode_size);
        #endif // DEBUG
        for (int i = 0; i < id_to_rn_sig[0].decode_size; ++i) {
            // if we are facing a branch instruction, save snapshot before proceeding
            rn_to_is_sig[1].valid = true;
            rn_to_is_sig[1].issue_size = 4;
            rn_to_is_sig[1].decoded[i] = id_to_rn_sig[0].decoded[i];
            if (id_to_rn_sig[0].decoded[i].is_branch) {
                ++current_jmp;
                #ifdef DEBUG
                printf("RN: branch instruction at pc %x\n arf to prf map:\n", id_to_rn_sig[0].decoded[i].pc);
                #endif // DEBUG
                for (int i = 0; i < 31; ++i) {
                    arf_prf_map[current_jmp][i] = arf_prf_map[current_jmp - 1][i];
                    #ifdef DEBUG
                    printf("arf %d prf %d\n", i, arf_prf_map[current_jmp][i]);
                    #endif // DEBUG
                }
            }
            
            if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_I) {
                rn_to_is_sig[1].renamed[i].rs2_phy = 0;
                // 1 dest (rd) 1 src (rs1)
                rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
                rn_to_is_sig[1].renamed[i].rs1_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs1_phy];
                rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
            } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_S) {
                // 2 src (rs1 rs2)
                rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
                rn_to_is_sig[1].renamed[i].rs1_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs1_phy];
                rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
                rn_to_is_sig[1].renamed[i].rs2_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs2_phy];
            } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_B) {
                // 2 src (rs1 rs2)
                rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
                rn_to_is_sig[1].renamed[i].rs1_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs1_phy];
                rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
                rn_to_is_sig[1].renamed[i].rs2_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs2_phy];
            } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_U) {
                rn_to_is_sig[1].renamed[i].rs1_phy = 0;
                rn_to_is_sig[1].renamed[i].rs2_phy = 0;
                // 1 dest (rd)
                rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
            } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_J) {
                rn_to_is_sig[1].renamed[i].rs1_phy = 0;
                rn_to_is_sig[1].renamed[i].rs2_phy = 0;
                // 1 dest (rd)
                rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
            } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_R) {
                // 1 dest (rd) 2 src (rs1 rs2)
                rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
                rn_to_is_sig[1].renamed[i].rs1_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs1_phy];
                rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
                rn_to_is_sig[1].renamed[i].rs2_ready = prf_ready[rn_to_is_sig[1].renamed[i].rs2_phy];
                rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
            }

            rn_to_is_sig[1].renamed[i].rs1_data = prf[rn_to_is_sig[1].renamed[i].rs1_phy];
            rn_to_is_sig[1].renamed[i].rs2_data = prf[rn_to_is_sig[1].renamed[i].rs2_phy];
        }
    }
    rn_to_id_sig[1].allow_in = true;
    rn_to_id_sig[1].rename_size = 4;
}