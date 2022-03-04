#include "RN.h"

extern struct id_to_rn id_to_rn_sig[2];
struct rn_to_id rn_to_id_sig[2];

struct rn_to_is rn_to_is_sig[2];
extern struct is_to_rn is_to_rn_sig[2];

// always assign r0 with prf[0]
uint32_t prf[PRF_SIZE];

// free list act as a stack
int prf_free_list[PRF_SIZE];
int prf_free_list_size = PRF_SIZE - 32;

int arf_prf_map[JMP_DEPTH][31];
int current_jmp = 0;

void RN_init() {
    for (int i = 0; i < PRF_SIZE; ++i) {
        prf[i] = 0xdeadbeef;
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

int alloc_dst(int dst) {
    assert(dst >= 0 && dst < 32);
    // find a free reg and map it as new dst register
    if (dst == 0) return 0;
    assert(prf_free_list_size > 0);
    int ret = prf_free_list[--prf_free_list_size];
    arf_prf_map[current_jmp][dst] = ret;
    return ret;
}

int commit_dst(int dst) {
    assert(dst >= 0 && dst < 32);
    // since instruction commited, we are free to recycle its previous map
    // todo
}

void RN_step() {
    // rename
    for (int i = 0; i < id_to_rn_sig[0].decode_size; ++i) {
        // if we are facing a branch instruction, save snapshot before proceeding
        if (id_to_rn_sig[0].decoded[i].is_branch) {
            ++current_jmp;
            for (int i = 0; i < 31; ++i) {
                arf_prf_map[current_jmp][i] = arf_prf_map[current_jmp - 1][i];
            }
        }
        
        if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_I) {
            // 1 dest (rd) 1 src (rs1)
            rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
            rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
        } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_S) {
            // 2 src (rs1 rs2)
            rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
            rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
        } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_B) {
            // 2 src (rs1 rs2)
            rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
            rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
        } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_U) {
            // 1 dest (rd)
            rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
        } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_J) {
            // 1 dest (rd)
            rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
        } else if (id_to_rn_sig[0].decoded[i].instr_type == TYPE_R) {
            // 1 dest (rd) 2 src (rs1 rs2)
            rn_to_is_sig[1].renamed[i].rs1_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs1);
            rn_to_is_sig[1].renamed[i].rs2_phy = alloc_src(id_to_rn_sig[0].decoded[i].rs2);
            rn_to_is_sig[1].renamed[i].rd_phy = alloc_dst(id_to_rn_sig[0].decoded[i].rd);
        }
    }    
}