#include "EX_impl.h"

extern struct ex_to_cmt ex_to_cmt_sig[2];
#include "interface.h"
void ALU_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    ex_to_cmt_sig[1].alu[offset].pc = decoded.pc;
    ex_to_cmt_sig[1].alu[offset].slot_valid = true;
    ex_to_cmt_sig[1].alu[offset].rd_valid = true;
    ex_to_cmt_sig[1].alu[offset].ard = decoded.rd;
    // arithmetic instructions
    switch (decoded.alu_type) {
        case ADD:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data + decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data + renamed.rs2_data;
            }
            break;
        case SUB:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data - decoded.imm;
            } else {
                
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data - renamed.rs2_data;
            }
            break;
        case SLT:
            if (decoded.instr_type == TYPE_I) {
                if ((int32_t)renamed.rs1_data < (int32_t)decoded.imm) {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 1;
                } else {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 0;
                }
            } else {
                if ((int32_t)renamed.rs1_data < (int32_t)renamed.rs2_data) {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 1;
                } else {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 0;
                }
            }
            break;
        case SLTU:
            if (decoded.instr_type == TYPE_I) {
                if (renamed.rs1_data < decoded.imm) {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 1;
                } else {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 0;
                }
            } else {
                if (renamed.rs1_data < renamed.rs2_data) {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 1;
                } else {
                    ex_to_cmt_sig[1].alu[offset].rd_data = 0;
                }
            }
            break;
        case OR:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data | decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data | renamed.rs2_data;
            }
            break;
        case XOR:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data ^ decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data ^ renamed.rs2_data;
            }
            break;
        case AND:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data & decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data & renamed.rs2_data;
            }
            break;
        case SLL:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data << decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data << (renamed.rs2_data & 0x1f);
            }
            break;
        case SRL:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data >> decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data >> (renamed.rs2_data & 0x1f);
            }
            break;
        case SRA:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> (renamed.rs2_data & 0x1f);
            }
            break;
        case LUI:
            ex_to_cmt_sig[1].alu[offset].rd_data = decoded.imm;
            break;
        case AUIPC:
            ex_to_cmt_sig[1].alu[offset].rd_data = decoded.imm + decoded.pc;
            break;
        default:
            break;
    }
}

void LSU_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // load/store instruction
}

void MDU_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // multiply/division instruction
}

extern struct jmp_redirectInfo jmp_redirect[2];

void JMP_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    bool taken = false;
    // jmp/branch instruction
    // by default we proceed without considering branch, so if branch, redirect
    switch (decoded.branch_type) {
        BEQ:
            if (renamed.rs1_data == renamed.rs2_data) {
                taken = true;
            }
            break;
        BNE:
            if (renamed.rs1_data != renamed.rs2_data) {
                taken = true;
            }
            break;
        BLT:
            if ((int32_t)renamed.rs1_data < (int32_t)renamed.rs2_data) {
                taken = true;
            }
            break;
        BGE:
            if ((int32_t)renamed.rs1_data > (int32_t)renamed.rs2_data) {
                taken = true;
            }
            break;
        BLTU:
            if (renamed.rs1_data < renamed.rs2_data) {
                taken = true;
            }
            break;
        BGEU:
            if (renamed.rs1_data > renamed.rs2_data) {
                taken = true;
            }
            break;
        JAL:
            taken = true;
            // with valid rd
            ex_to_cmt_sig[1].jmp[offset].rd_valid = true;
            ex_to_cmt_sig[1].jmp[offset].rd_data = decoded.pc + 4;
            break;
        JALR:
            // taken = true;
            // directly update redirect info
            jmp_redirect[1].instr_idx = decoded.instr_idx + 1;
            jmp_redirect[1].redirect_pc = renamed.rs1_data + decoded.imm;
            // with valid rd
            ex_to_cmt_sig[1].jmp[offset].rd_valid = true;
            ex_to_cmt_sig[1].jmp[offset].rd_data = decoded.pc + 4;
            break;
    }
    if (taken) {
        jmp_redirect[1].instr_idx = decoded.instr_idx + 1;
        jmp_redirect[1].redirect_pc = decoded.pc + decoded.imm;
    }
    
}