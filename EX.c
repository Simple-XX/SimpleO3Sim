#include "EX.h"
#include <assert.h>
#include "EX_impl.h"

extern struct is_to_ex is_to_ex_sig[2];
struct is_to_rn ex_to_is_sig[2];

struct ex_to_cmt ex_to_cmt_sig[2];
extern struct cmt_to_ex cmt_to_ex_sig[2];


struct jmp_redirectInfo jmp_to_is_sig[2];

struct ex_jmpInfo jmp_pipeline[JMP_DELAY][JMP_SIZE + 1];
struct ex_aluInfo alu_pipeline[ALU_DELAY][ALU_SIZE + 1];
struct ex_mduInfo mdu_pipeline[MDU_DELAY][MDU_SIZE + 1];
struct ex_lsuInfo lsu_pipeline[LSU_DELAY][LSU_SIZE + 1];

void jmpUnit() {
    if (jmp_to_is_sig[0].redirect_valid) {
        // if flush, remove all instructions after flush point
        for (int i = 0; i <= JMP_SIZE; ++i) {
            for (int j = 0; j < JMP_DELAY; ++j) {
                if (jmp_pipeline[j][i].decoded.instr_idx > jmp_to_is_sig[0].instr_idx) {
                    jmp_pipeline[j][i].valid = false;
                }
            }
        }
    }

    // firstly deal with the result that will come out of this stage
    ex_to_cmt_sig[1].jmp_size = 0;
    for (int i = 0; i <= JMP_SIZE; ++i) {
        if (jmp_pipeline[JMP_DELAY - 1][i].valid) {
            ex_to_cmt_sig[1].valid = true;
            // if we have a valid branch/jmp instruction
            JMP_calc(jmp_pipeline[JMP_DELAY - 1][i].decoded, 
                     jmp_pipeline[JMP_DELAY - 1][i].renamed,
                     i);
            ex_to_cmt_sig[1].jmp[i].idx = jmp_pipeline[JMP_DELAY - 1][i].decoded.instr_idx;
            ++ex_to_cmt_sig[1].jmp_size;
            ex_to_cmt_sig[1].jmp[i].renamed = jmp_pipeline[JMP_DELAY - 1][i].renamed;
        } else {
            // may have flush, no ops
            // valid signals must be consecutive
            // ex_to_cmt_sig[1].jmp_size = i;
            // break;
        }
    }
    
    // every other stages move forward
    for (int i = 0; i < JMP_DELAY - 1; ++i) {
        for (int j = 0; j < JMP_SIZE; ++j) {
            jmp_pipeline[i+1][j] = jmp_pipeline[i][j];
        }
    }

    // if there is a branch/jmp instruction
    if (is_to_ex_sig[0].valid && is_to_ex_sig[0].jmp_size) {
        assert(is_to_ex_sig[0].jmp_size <= JMP_SIZE);
        // accept incoming instructions
        for (int i = 0; i < is_to_ex_sig[0].jmp_size; ++i) {
            assert(is_to_ex_sig[0].jmp[i].decoded.instr_type == TYPE_B || is_to_ex_sig[0].jmp[i].decoded.instr_type == TYPE_J);
            jmp_pipeline[0][i] = is_to_ex_sig[0].jmp[i];
        }
    }
    for (int i = is_to_ex_sig[0].jmp_size; i < JMP_SIZE; ++i) {
        jmp_pipeline[0][i].valid = false;
    }
}

void lsUnit() {
    // firstly deal with the result that will come out of this stag
    for (int i = 0; i <= LSU_SIZE; ++i) {
        if (lsu_pipeline[LSU_DELAY - 1][i].valid) {
            // we have a valid request
        } else {
            // valid signals must be consecutive
        }
    }
    // every other stages move forward
    for (int i = 0; i < LSU_DELAY - 1; ++i) {
        for (int j = 0; j < LSU_SIZE; ++j) {
            lsu_pipeline[i+1][j] = lsu_pipeline[i][j];
        }
    }
    // if there is a load/store instruction
    if (is_to_ex_sig[0].valid && is_to_ex_sig[0].lsu_size) {
        assert(is_to_ex_sig[0].lsu_size <= LSU_SIZE);
        // accept incoming instructions
        for (int i = 0; i < is_to_ex_sig[0].lsu_size; ++i) {
            lsu_pipeline[0][i] = is_to_ex_sig[0].lsu[i];
        }
    }
    for (int i = is_to_ex_sig[0].lsu_size; i < LSU_SIZE; ++i) {
        lsu_pipeline[0][i].valid = false;
    }
}

void aluUnit() {
    // if flush, remove all instructions after flush point
    if (jmp_to_is_sig[0].redirect_valid) {
        for (int i = 0; i <= ALU_SIZE; ++i) {
            for (int j = 0; j < ALU_DELAY; ++j) {
                if (alu_pipeline[j][i].decoded.instr_idx > jmp_to_is_sig[0].instr_idx) {
                    alu_pipeline[j][i].valid = false;
                }
            }
        }
    }
    

    // firstly deal with the result that will come out of this stage
    ex_to_cmt_sig[1].alu_size = 0;
    for (int i = 0; i <= ALU_SIZE; ++i) {
        if (alu_pipeline[ALU_DELAY - 1][i].valid) {
            // we have a valid request
            ex_to_cmt_sig[1].valid = true;
            ALU_calc(alu_pipeline[ALU_DELAY - 1][i].decoded,
                     alu_pipeline[ALU_DELAY - 1][i].renamed,
                     i
            );
            ex_to_cmt_sig[1].alu[i].idx = alu_pipeline[ALU_DELAY - 1][i].decoded.instr_idx;
            ++ex_to_cmt_sig[1].alu_size;
            ex_to_cmt_sig[1].alu[i].renamed = alu_pipeline[ALU_DELAY - 1][i].renamed;
        } else {
            // may have flush, no ops
            // valid signals must be consecutive
            // break;
        }
    }

    for (int i = 0; i < ALU_DELAY - 1; ++i) {
        for (int j = 0; j < ALU_SIZE; ++j) {
            alu_pipeline[i+1][j] = alu_pipeline[i][j];
        }
    }

    // if there is an arithmetic instruction
    if (is_to_ex_sig[0].valid && is_to_ex_sig[0].alu_size) {
        assert(is_to_ex_sig[0].alu_size <= ALU_SIZE);
        for (int i = 0; i < is_to_ex_sig[0].alu_size; ++i) {
            alu_pipeline[0][i] = is_to_ex_sig[0].alu[i];
        }
        
    }
}

void mdUnit() {
    // if flush, remove all instructions after flush point
    for (int i = 0; i <= ALU_SIZE; ++i) {
        for (int j = 0; j < ALU_DELAY; ++j) {
            if (mdu_pipeline[j][i].decoded.instr_idx > jmp_to_is_sig[0].instr_idx) {
                mdu_pipeline[j][i].valid = false;
            }
        }
    }

    // foo implementation here
    ex_to_cmt_sig[1].mdu_size = 0;
    for (int i = 0; i <= MDU_SIZE; i++) {
        if (mdu_pipeline[MDU_DELAY - 1][i].valid) {
            // we have a valid request
            ex_to_cmt_sig[1].valid = true;
            MDU_calc(mdu_pipeline[ALU_DELAY - 1][i].decoded,
                     mdu_pipeline[ALU_DELAY - 1][i].renamed,
                     i
            );
            ex_to_cmt_sig[1].mdu[i].idx = mdu_pipeline[MDU_DELAY - 1][i].decoded.instr_idx;
            ex_to_cmt_sig[1].mdu[i].renamed = mdu_pipeline[MDU_DELAY - 1][i].renamed;
        }
    }
}

void EX_step() {
    #ifdef DEBUG
    printf("EX: alu %d instrs mdu %d instrs lsu %d instrs jmp %d instrs\n", is_to_ex_sig[0].alu_size, is_to_ex_sig[0].mdu_size, is_to_ex_sig[0].lsu_size, is_to_ex_sig[0].jmp_size);
    #endif // DEBUG
    // execuction
    ex_to_cmt_sig[1].valid = false;
    aluUnit();
    lsUnit();
    jmpUnit();
    mdUnit();
}