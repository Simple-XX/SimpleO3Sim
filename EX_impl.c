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
            printf("add PC %x rd %d data = %x rs1 %d rs2 %d imm %d\n", decoded.pc, decoded.rd, ex_to_cmt_sig[1].alu[offset].rd_data, renamed.rs1_phy, renamed.rs2_phy, decoded.imm);
            break;
        case ADDW:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)((int32_t)renamed.rs1_data + (int32_t)decoded.imm);
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)((int32_t)renamed.rs1_data + (int32_t)renamed.rs2_data);
            }
            break;
        case SUB:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data - decoded.imm;
            } else {
                
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data - renamed.rs2_data;
            }
            break;
        case SUBW:
            if (decoded.instr_type == TYPE_I) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)((int32_t)renamed.rs1_data - (int32_t)decoded.imm);
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)((int32_t)renamed.rs1_data - (int32_t)renamed.rs2_data);
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
                printf("slli imm %d\n", decoded.imm);
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data << (renamed.rs2_data & 0x1f);
            }
            break;
        case SLLW:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data << decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data << (renamed.rs2_data & 0x1f);
            }
            break;
        case SRL:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data >> decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = renamed.rs1_data >> (renamed.rs2_data & 0x1f);
            }
            break;
        case SRLW:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> (renamed.rs2_data & 0x1f);
            }
            break;
        case SRA:
            if (decoded.shift_imm) {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> decoded.imm;
            } else {
                ex_to_cmt_sig[1].alu[offset].rd_data = (int32_t)renamed.rs1_data >> (renamed.rs2_data & 0x1f);
            }
            break;
        case SRAW:
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

void CSR_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // CSR instruction

}

void MDU_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // multiply/division instruction
    ex_to_cmt_sig[1].mdu[offset].pc = decoded.pc;
    ex_to_cmt_sig[1].mdu[offset].slot_valid = true;
    ex_to_cmt_sig[1].mdu[offset].rd_valid = true;
    ex_to_cmt_sig[1].mdu[offset].ard = decoded.rd;

    switch (decoded.mdu_type) {
        case MUL:
            ex_to_cmt_sig[1].mdu[offset].rd_data = renamed.rs1_data * renamed.rs2_data;
            break;
        case MULH:
            ex_to_cmt_sig[1].mdu[offset].rd_data = (uint64_t)((int32_t)renamed.rs1_data * (int32_t)renamed.rs2_data) >> 32;
            break;
        case MULHSU:
            ex_to_cmt_sig[1].mdu[offset].rd_data = (uint64_t)((int32_t)renamed.rs1_data * (uint64_t)renamed.rs2_data) >> 32;
            break;
        case MULHU:
            ex_to_cmt_sig[1].mdu[offset].rd_data = ((uint64_t)renamed.rs1_data * (uint64_t)renamed.rs2_data) >> 32;
            break;
        case DIV:
            ex_to_cmt_sig[1].mdu[offset].rd_data = (int32_t)renamed.rs1_data / (int32_t)renamed.rs2_data;
            break;
        case DIVU:
            ex_to_cmt_sig[1].mdu[offset].rd_data = renamed.rs1_data / renamed.rs2_data;
            break;
        case REM:
            ex_to_cmt_sig[1].mdu[offset].rd_data = (int32_t)renamed.rs1_data % (int32_t)renamed.rs2_data;
            break;
        case REMU:
            ex_to_cmt_sig[1].mdu[offset].rd_data = renamed.rs1_data % renamed.rs2_data;
            break;
        default:
            assert(1);
    }
}

extern struct jmp_redirectInfo jmp_to_is_sig[2];

void JMP_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    ex_to_cmt_sig[1].jmp[offset].pc = decoded.pc;
    ex_to_cmt_sig[1].jmp[offset].slot_valid = true;
    ex_to_cmt_sig[1].jmp[offset].ard = decoded.rd;
    bool taken = false;
    // jmp/branch instruction
    // by default we proceed without considering branch, so if branch, redirect
    switch (decoded.branch_type) {
        case BEQ:
            if (renamed.rs1_data == renamed.rs2_data) {
                taken = true;
            }
            break;
        case BNE:
            if (renamed.rs1_data != renamed.rs2_data) {
                taken = true;
            }
            break;
        case BLT:
            if ((int32_t)renamed.rs1_data < (int32_t)renamed.rs2_data) {
                taken = true;
            }
            break;
        case BGE:
            if ((int32_t)renamed.rs1_data > (int32_t)renamed.rs2_data) {
                taken = true;
            }
            break;
        case BLTU:
            if (renamed.rs1_data < renamed.rs2_data) {
                taken = true;
            }
            break;
        case BGEU:
            if (renamed.rs1_data > renamed.rs2_data) {
                taken = true;
            }
            break;
        case JAL:
            taken = true;
            // with valid rd
            ex_to_cmt_sig[1].jmp[offset].rd_valid = true;
            ex_to_cmt_sig[1].jmp[offset].rd_data = decoded.pc + 4;
            break;
        case JALR:
            printf("jalr pc %lx rs1 %d rs2 %d\n", decoded.pc, renamed.rs1_phy, renamed.rs2_phy);
            taken = true;
            // directly update redirect info
            jmp_to_is_sig[1].instr_idx = decoded.instr_idx;
            printf("imm %lx rs1_data %lx\n", decoded.imm, renamed.rs1_data);
            jmp_to_is_sig[1].redirect_pc = renamed.rs1_data + decoded.imm;
            // with valid rd
            ex_to_cmt_sig[1].jmp[offset].rd_valid = true;
            ex_to_cmt_sig[1].jmp[offset].rd_data = decoded.pc + 4;
            break;
        default:
            printf("Unknown branch type %d\n", decoded.branch_type);
            assert(0);
            break;
    }
    
    if (taken) {
        jmp_to_is_sig[1].redirect_valid = true;
        jmp_to_is_sig[1].instr_idx = decoded.instr_idx;
        if (decoded.branch_type != JALR) {
            jmp_to_is_sig[1].redirect_pc = decoded.pc + decoded.imm;
        }
        
        jmp_to_is_sig[1].current_jmp = renamed.current_jmp;
    } else {
        jmp_to_is_sig[1].redirect_valid = false;
    }
    
}