#ifndef INTERFACE_H 
#define INTERFACE_H

#include <stdbool.h>
#include <stdint.h>
// instruction and data ram interface

struct ram_req {
    bool valid;
    bool wr;
    uint64_t size;  // 0: 1 byte 1: 2 bytes 2: 4 bytes 3: 8 bytes 4: 16 bytes
    uint64_t addr;  // should always be false for instr
    uint64_t wdata; // should always be ignored by instr
};

struct ram_resp {
    uint32_t rdata[4];
};

// IF to ID

struct if_to_id {
    bool valid;

    int instr_size;
    uint32_t instr[4];
    uint64_t fetch_pc;
};

struct id_to_if {
    bool allow_in;
    int instr_allow_size;
};

// ID to RN

enum LS_TYPE {LW, LH, LB, SW, SH, SB, LHU, LBU, LD, SD, LWU};
enum ALU_TYPE {ADD, ADDW, SUB, SUBW, SLT, SLTU, OR, XOR, AND, SLL, SLLW, SRLW, SRAW, SRL, SRA, LUI, AUIPC};
enum MDU_TYPE {MUL, MULH, MULHSU, MULHU, DIV, DIVU, REMU, REM};
enum instr_type {TYPE_B, TYPE_S, TYPE_I, TYPE_J, TYPE_R, TYPE_U};
enum branch_type {BEQ, BNE, BLT, BGE, BLTU, BGEU, JAL, JALR};
enum CSR_TYPE {CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI};

struct decode_info {
// every instruction has a decode info
    bool is_branch;
    bool is_lsu;
    bool is_load, is_store;
    bool is_csr;
    enum CSR_TYPE csr_type;
    int lsu_type;
    bool is_alu;
    bool is_mdu;
    enum ALU_TYPE alu_type;
    enum MDU_TYPE mdu_type;
    int rs1, rs2, rd;
    uint64_t imm;
    uint64_t pc;
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
    int current_jmp;
    int rs1_phy, rs2_phy;
    bool rs1_ready, rs2_ready;
    struct int_pair rd_phy;
    int arch_rd;
    uint64_t rs1_data, rs2_data;
    bool is_csr;
    uint64_t csr_instr;
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

struct ex_csrInfo {
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
#define ALU_SIZE 4
#define MDU_SIZE 2
#define LSU_SIZE 4
#define CSR_SIZE 1

#define ALU_DELAY 1
#define JMP_DELAY 1
#define MDU_DELAY 2
#define LSU_DELAY 1
#define CSR_DELAY 1

struct is_to_ex {
    bool valid;

    // to every execution unit
    int jmp_size;
    int alu_size;
    int mdu_size;
    int lsu_size;
    int csr_size;
    struct ex_jmpInfo jmp[JMP_SIZE];
    struct ex_aluInfo alu[ALU_SIZE];
    struct ex_mduInfo mdu[MDU_SIZE];
    struct ex_lsuInfo lsu[LSU_SIZE];
    struct ex_csrInfo csr[CSR_SIZE];
};

struct ex_to_is {
    bool jmp_allowin[JMP_SIZE];
    bool alu_allowin[ALU_SIZE];
    bool mdu_allowin[MDU_SIZE];
    bool lsu_allowin[LSU_SIZE];
};

// EX to CMT
struct commitInfo {
    bool slot_valid;

    uint64_t pc; // for debug only
    int ard; // for debug only
    
    struct rename_info renamed;
    bool rd_valid;
    uint64_t rd_data;

    bool store_valid;
    uint64_t store_data;
    uint64_t idx;
};

struct jmp_redirectInfo {
    // redirect with instruction count number
    bool redirect_valid;
    uint64_t redirect_pc;
    uint64_t instr_idx;
    int current_jmp;
};

struct ex_to_cmt {
    bool valid;

    int jmp_size, alu_size, mdu_size, lsu_size, csr_size;
    struct commitInfo jmp[JMP_SIZE];
    struct commitInfo alu[ALU_SIZE];
    struct commitInfo mdu[MDU_SIZE];
    struct commitInfo lsu[LSU_SIZE];
    struct commitInfo csr[CSR_SIZE];
};

struct cmt_to_ex {
    bool valid;

    // may need more info
};

struct wakeup_info {
    // if an instruction overwrites the dst, we are free to recycle the previous one
    int recycle_dst;
    bool is_csr;
    uint64_t csr_instr;
};

#define COMMIT_SIZE 4
#define ROB_SIZE 200

struct cmt_wakeup_info {
    bool valid;

    int commit_size;
    struct wakeup_info committed[COMMIT_SIZE];
};

struct lsu_to_clint {
    bool valid;
    uint64_t addr;
    bool write;
    uint64_t data;
    uint64_t size;
};

struct clint_to_lsu {
    bool valid;
    uint64_t rdata;
};

struct clint_interrupt {
    bool valid;
};

enum CSR_OP {CSRW, CSRS, CSRC};

struct csr_req {
    // foo
    enum CSR_OP op;
    uint32_t csrAddr;
    uint64_t rs_data;
};
struct csr_resp {
    // foo
    uint64_t rd_data;
};

#endif