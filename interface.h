#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdbool.h>
#include <stdint.h>
// instruction and data ram interface

struct ram_req {
    bool valid;
    bool wr;
    uint32_t size;  // 0: 1 byte 1: 2 bytes 2: 4 bytes 3: 8 bytes 4: 16 bytes
    uint32_t addr;  // should always be false for instr
    uint32_t wdata; // should always be ignored by instr
};

struct ram_resp {
    uint32_t rdata[4];
};

// IF to ID

struct if_to_id {
    bool valid;

    int instr_size;
    uint32_t instr[4];
    uint32_t fetch_pc;
};

struct id_to_if {
    bool allow_in;
    int instr_allow_size;
};

// ID to RN

enum LS_TYPE {LW, LH, LB, SW, SH, SB, LHU, LBU};
enum ALU_TYPE {ADD, SUB, SLT, SLTU, OR, XOR, AND, SLL, SRL, SRA, LUI, AUIPC};
enum instr_type {TYPE_B, TYPE_S, TYPE_I, TYPE_J, TYPE_R, TYPE_U};
enum branch_type {BEQ, BNE, BLT, BGE, BLTU, BGEU, JAL, JALR};

struct decode_info {
// every instruction has a decode info
    bool is_branch;
    bool is_lsu;
    bool is_load, is_store;
    int lsu_type;
    bool is_alu;
    int alu_type;
    int rs1, rs2, rd;
    uint32_t imm;
    uint32_t pc;
    int instr_type;
    int branch_type;
    bool shift_imm; // only for identification of s{r/l}{l/a}i
    uint64_t instr_idx;
};

struct id_to_rn {
    bool valid;

    int decode_size;
    struct decode_info decoded[4];
};

struct rn_to_id {
    bool allow_in;

    int rename_size;
};

struct int_pair {
    int a, b;
};

// RN to IS
struct rename_info {
    int rs1_phy, rs2_phy;
    bool rs1_ready, rs2_ready;
    struct int_pair rd_phy;
    uint32_t rs1_data, rs2_data;
};

struct rn_to_is {
    bool valid;

    int issue_size;
    struct decode_info decoded[4];
    struct rename_info renamed[4];
};

struct is_to_rn {
    bool allow_in;
    
    int issue_size;
};

// RN to EX

struct ex_jmpInfo {
    bool valid;
    struct decode_info decoded;
    struct rename_info renamed;
};

struct ex_aluInfo {
    bool valid;
    struct decode_info decoded;
    struct rename_info renamed;
};

struct ex_mduInfo {
    bool valid;
    struct decode_info decoded;
    struct rename_info renamed;
};

struct ex_lsuInfo {
    bool valid;
    struct decode_info decoded;
    struct rename_info renamed;
};

#define JMP_SIZE 1
#define ALU_SIZE 3
#define MDU_SIZE 2
#define LSU_SIZE 1

#define ALU_DELAY 1
#define JMP_DELAY 1
#define MDU_DELAY 2
#define LSU_DELAY 1

struct is_to_ex {
    bool valid;

    // to every execution unit
    int jmp_size;
    int alu_size;
    int mdu_size;
    int lsu_size;
    struct ex_jmpInfo jmp[JMP_SIZE];
    struct ex_aluInfo alu[ALU_SIZE];
    struct ex_mduInfo mdu[MDU_SIZE];
    struct ex_lsuInfo lsu[LSU_SIZE];
};

struct ex_to_is {
    bool jmp_allowin[JMP_SIZE];
    bool alu_allowin[ALU_SIZE];
    bool mdu_allowin[MDU_SIZE];
    bool lsu_allowin[LSU_SIZE];
};

// EX to CMT

struct jmp_commitInfo {
    // foo
    struct rename_info renamed;
    bool rd_valid;
    uint32_t rd_data;
};

struct alu_commitInfo {
    struct rename_info renamed;
    bool rd_valid;
    uint32_t rd_data;
};

struct mdu_commitInfo {
    struct rename_info renamed;
    bool rd_valid;
    uint32_t rd_data;
};

struct lsu_commitInfo {
    struct rename_info renamed;
    bool rd_valid;
    uint32_t rd_data;

    bool store_valid;
    uint32_t store_data;
    uint64_t idx;
};

struct jmp_redirectInfo {
    // redirect with instruction count number
    uint32_t redirect_pc;
    uint64_t instr_idx;
};

struct ex_to_cmt {
    bool valid;

    int jmp_size, alu_size, mdu_size, lsu_size;
    struct jmp_commitInfo jmp[JMP_SIZE];
    struct alu_commitInfo alu[ALU_SIZE];
    struct mdu_commitInfo mdu[MDU_SIZE];
    struct lsu_commitInfo lsu[LSU_SIZE];
};

struct cmt_to_ex {
    bool valid;

    // may need more info
};

struct commit_info {
    // if an instruction overwrites the dst, we are free to recycle the previous one
    int recycle_dst;
};

struct cmt_wakeup_info {
    bool valid;

    int commit_size;
    struct commit_info committed[4];
};

#endif