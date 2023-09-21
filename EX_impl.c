#include "EX_impl.h"
#include "csr.h"

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

extern char* ram;
extern uint16_t *ram_half;
extern uint32_t *ram_word;
extern uint64_t *ram_double;

void LSU_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // load/store instruction
    ex_to_cmt_sig[1].lsu[offset].pc = decoded.pc;
    ex_to_cmt_sig[1].lsu[offset].slot_valid = true;
    ex_to_cmt_sig[1].lsu[offset].ard = decoded.rd;
    if (decoded.is_load) {
        ex_to_cmt_sig[1].lsu[offset].rd_valid = true;
    }
    printf("decode imm %d rs1 %lx\n", decoded.imm, renamed.rs1_data);

    switch (decoded.lsu_type) {
        case LB:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (int8_t)ram[renamed.rs1_data + decoded.imm - 0x80000000];
            break;
        case LH:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (int16_t)ram_half[(renamed.rs1_data + decoded.imm - 0x80000000) / 2];
            break;
        case LW:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (int32_t)ram_word[(renamed.rs1_data + decoded.imm - 0x80000000) / 4];
            break;
        case LBU:
            ex_to_cmt_sig[1].lsu[offset].rd_data = ram[renamed.rs1_data + decoded.imm - 0x80000000];
            break;
        case LHU:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (uint16_t)ram_half[(renamed.rs1_data + decoded.imm - 0x80000000) / 2];
            break;
        case LWU:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (uint32_t)ram_word[(renamed.rs1_data + decoded.imm - 0x80000000) / 4];
            break;
        case LD:
            ex_to_cmt_sig[1].lsu[offset].rd_data = (uint64_t)ram_double[(renamed.rs1_data + decoded.imm - 0x80000000) / 8];
            printf("LD\n");
            break;
        case SB:
            ram[renamed.rs1_data + decoded.imm - 0x80000000] = renamed.rs2_data;
            break;
        case SH:
            ram_half[(renamed.rs1_data + decoded.imm - 0x80000000) / 2] = renamed.rs2_data;
            break;
        case SW:
            ram_word[(renamed.rs1_data + decoded.imm - 0x80000000) / 4] = renamed.rs2_data;
            break;
        case SD:
            printf("SD\n");
            ram_double[(renamed.rs1_data + decoded.imm - 0x80000000) / 8] = renamed.rs2_data;
            break;
        default:
            break;
    }
}
/*
void CSR_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    // CSR instruction

}*/

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

void CSR_calc(struct decode_info decoded, struct rename_info renamed, int offset) {
    ex_to_cmt_sig[1].csr[offset].pc = decoded.pc;
    ex_to_cmt_sig[1].csr[offset].slot_valid = true;
    ex_to_cmt_sig[1].csr[offset].rd_valid = true;
    ex_to_cmt_sig[1].csr[offset].ard = decoded.rd;

    int csr_addr = decoded.imm;
    uint64_t *csr_src = get_csr(csr_addr);
    uint64_t rs_data, csr_data = *csr_src;

    switch (decoded.csr_type) {
        case CSRRW:
            rs_data = renamed.rs1_data;
            *csr_src = rs_data;
            printf("CSRRW rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
        case CSRRC:
            rs_data = renamed.rs1_data;
            *csr_src = *csr_src ^ rs_data;
            printf("CSRRC rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
        case CSRRS:
            rs_data = renamed.rs1_data;
            *csr_src = *csr_src | rs_data;
            printf("CSRRS rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
        case CSRRWI:
            rs_data = decoded.rs1;
            *csr_src = rs_data;
            printf("CSRRWI rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
        case CSRRCI:
            rs_data = decoded.rs1;
            *csr_src = *csr_src ^ rs_data;
            printf("CSRRCI rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
        case CSRRSI:
            rs_data = decoded.rs1;
            *csr_src = *csr_src | rs_data;
            printf("CSRRSI rs_data %lx csr_src %lx rd_data %lx\n", rs_data, *csr_src, csr_data);
            break;
    }
    ex_to_cmt_sig[1].csr[offset].rd_data = csr_data;
}