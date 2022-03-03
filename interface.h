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
};

struct id_to_if {
    bool allow_in;
};

// ID to RN

struct decode_info {
    // every instruction has a decode info
};

struct id_to_rn {
    bool valid;

    int decode_size;
    struct decode_info decoded[4];
};

struct rn_to_id {
    bool allow_in;
};


// RN to EX

struct ex_jmpInfo {
    // foo
};

struct ex_aluInfo {
    // foo
};

struct ex_mduInfo {
    // foo
};

struct ex_lsuInfo {
    // foo
};

struct rn_to_ex {
    bool valid;

    // to every execution unit
    struct ex_jmpInfo jmp;
    struct ex_aluInfo alu[3];
    struct ex_mduInfo mdu[2];
    struct ex_lsuInfo lsu;
};

struct ex_to_rn {
    bool jmp_allowin;
    bool alu_allowin[3];
    bool mdu_allowin[2];
    bool lsu_allowin;
};

// EX to CMT

struct jmp_commitInfo {
    // foo
};

struct alu_commitInfo {
    // 
};

struct mdu_commitInfo {
    // 
};

struct lsu_commitInfo {
    // foo
};

struct ex_to_cmt {
    bool valid;

    struct jmp_commitInfo jmp;
    struct alu_commitInfo alu;
    struct mdu_commitInfo mdu;
    struct lsu_commitInfo lsu;
};

struct cmt_to_ex {
    bool valid;

    // may need more info
};

#endif