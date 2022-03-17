#include "EX.h"
#include <assert.h>

extern struct is_to_ex is_to_ex_sig[2];
struct is_to_rn ex_to_is_sig[2];

struct ex_to_cmt ex_to_cmt_sig[2];
extern struct cmt_to_ex cmt_to_ex_sig[2];


struct jmp_redirectInfo jmp_redirect[2];

struct ex_jmpInfo jmp_pipeline[JMP_DELAY][JMP_SIZE];
struct ex_aluInfo alu_pipeline[ALU_DELAY][ALU_SIZE];
struct ex_mduInfo mdu_pipeline[MDU_DELAY][MDU_SIZE];
struct ex_lsuInfo lsu_pipeline[LSU_DELAY][LSU_SIZE];

void jmpUnit() {
    // firstly deal with the result that will come out of this stage
    for (int i = 0; i < JMP_SIZE; ++i) {
        if (jmp_pipeline[JMP_DELAY - 1][i].valid) {
            // if we have a valid branch/jmp instruction
            if (is_to_ex_sig[0].jmp[i].decoded.instr_type == TYPE_B) {
                // is ordinary branch

            } else if (is_to_ex_sig[0].jmp[i].decoded.instr_type == TYPE_J) {
                // jal
            } else if (is_to_ex_sig[0].jmp[i].decoded.instr_type == TYPE_I) {
                // jalr
            }
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
            assert(is_to_ex_sig[0].jmp[i].decoded.is_branch);
            jmp_pipeline[0][i] = is_to_ex_sig[0].jmp[i];
        }
    }
    for (int i = is_to_ex_sig[0].jmp_size; i < JMP_SIZE; ++i) {
        jmp_pipeline[0][i].valid = false;
    }
}

void lsUnit() {
    // firstly deal with the result that will come out of this stag
    for (int i = 0; i < LSU_SIZE; ++i) {
        if (lsu_pipeline[LSU_DELAY - 1][i].valid) {
            // we have a valid request
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
    // firstly deal with the result that will come out of this stage
    for (int i = 0; i < ALU_SIZE; ++i) {
        if (lsu_pipeline[ALU_DELAY - 1][i].valid) {
            // we have a valid request
        }
    }

    // if there is an arithmetic instruction
    if (is_to_ex_sig[0].valid && is_to_ex_sig[0].alu_size) {
        assert(is_to_ex_sig[0].alu_size <= ALU_SIZE);
        // firstly deal with the result that will come out of this stage
    }
}

void EX_step() {
    // execuction
}