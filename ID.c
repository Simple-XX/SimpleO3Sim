#include "interface.h"
#include "ID.h"
#include "common.h"

extern struct if_to_id if_to_id_sig[2];
struct id_to_if id_to_if_sig[2];

struct id_to_rn id_to_rn_sig[2];
extern struct rn_to_id rn_to_id_sig[2];

extern struct jmp_redirectInfo jmp_to_is_sig[2];

static uint64_t decode_queue[DECODE_QUEUE_SIZE];
static uint64_t decode_pc[DECODE_QUEUE_SIZE];
static int decode_queue_start = 0, decode_queue_end = 0, queue_size = 0;

uint64_t global_instr_idx;

static uint64_t imm_expansion(uint64_t imm, int from_bit) {
    // dup since from_bit
    uint64_t extractor = 1 << from_bit & imm;
    uint64_t ret = 0;
    for (int i = 0; i < 64 - from_bit; ++i) {
        ret = (ret << 1) | extractor;
    }
    ret |= imm;
    return ret;
}

struct decode_info decode(const uint32_t instr) {
    struct decode_info ret;
    uint32_t opcode, rs1, rs2, rd, funct7, funct3, imm_i, imm_s, imm_b, imm_u, imm_j;
    opcode = instr & 0x7f;
    rs1 = (instr >> 15) & 0x1f;
    rs2 = (instr >> 20) & 0x1f;
    rd = (instr >> 7) & 0x1f;
    funct7 = (instr >> 25) & 0x7f;
    funct3 = (instr >> 12) & 0x7;
    imm_i = (instr >> 20) & 0xfff;
    imm_s = ((instr >> 7) & 0x1f) | (((instr >> 25) & 0x7f) << 5);
    imm_b = ((instr >> 31) << 12) | (((instr >> 25) & 0x3f) << 5) | (((instr >> 8) & 0xf) << 1) | (((instr >> 7) & 0x1) << 11);
    imm_u = (instr >> 12) << 12;
    imm_j = ((instr >> 31) << 20) | (((instr >> 12) & 0xff) << 12) | (((instr >> 20) & 1) << 11) | (((instr >> 21) & 0x3ff) << 1);

    bool lui, auipc, jal, jalr, beq, bne, blt, bge, bltu, bgeu;
    bool lb, lh, lw, lbu, lhu, lwu, ld, sb, sh, sw, sd, addi, slti, sltiu;
    bool xori, ori, andi, slli, srli, srai, add, sub, sll, slt;
    bool sltu, xor, srl, sra, or, and;
    bool mul, mulh, mulhu, mulhsu, div, divu, rem, remu;
    bool addiw, slliw, srliw, sraiw;
    bool addw, subw, sllw, srlw, sraw;
    bool csrrs, csrrw, csrrc, csrrsi, csrrwi, csrrci;
    

    lui = opcode == 0x37;
    auipc = opcode == 0x17;
    jal = opcode == 0x6f;
    jalr = opcode == 0x67 && funct3 == 0x0;
    beq = opcode == 0x63 && funct3 == 0x0;
    bne = opcode == 0x63 && funct3 == 0x1;
    blt = opcode == 0x63 && funct3 == 0x4;
    bge = opcode == 0x63 && funct3 == 0x5;
    bltu = opcode == 0x63 && funct3 == 0x6;
    bgeu = opcode == 0x63 && funct3 == 0x7;
    lb = opcode == 0x3 && funct3 == 0x0;
    lh = opcode == 0x3 && funct3 == 0x1;
    lw = opcode == 0x3 && funct3 == 0x2;
    lbu = opcode == 0x3 && funct3 == 0x4;
    lhu = opcode == 0x3 && funct3 == 0x5;
    lwu = opcode == 0x3 && funct3 == 0x6;
    ld = opcode == 0x3 && funct3 == 0x3;
    sb = opcode == 0x23 && funct3 == 0x0;
    sh = opcode == 0x23 && funct3 == 0x1;
    sw = opcode == 0x23 && funct3 == 0x2;
    sd = opcode == 0x23 && funct3 == 0x3;
    addi = opcode == 0x13 && funct3 == 0x0;
    addiw = opcode == 0x1b && funct3 == 0x0;
    slti  = opcode == 0x13 && funct3 == 0x02;
    sltiu = opcode == 0x13 && funct3 == 0x03;
    xori  = opcode == 0x13 && funct3 == 0x04;
    ori   = opcode == 0x13 && funct3 == 0x06;
    andi  = opcode == 0x13 && funct3 == 0x07;
    slli  = opcode == 0x13 && funct3 == 0x01;
    slliw = opcode == 0x1b && funct3 == 0x01;
    srli  = opcode == 0x13 && funct3 == 0x05 && funct7 == 0x00;
    srliw = opcode == 0x1b && funct3 == 0x05 && funct7 == 0x00;
    srai  = opcode == 0x13 && funct3 == 0x05 && funct7 == 0x20;
    sraiw = opcode == 0x1b && funct3 == 0x05 && funct7 == 0x20;
    add   = opcode == 0x33 && funct3 == 0x00 && funct7 == 0x00;
    addw  = opcode == 0x3b && funct3 == 0x00 && funct7 == 0x00;
    sub   = opcode == 0x33 && funct3 == 0x00 && funct7 == 0x20;
    subw  = opcode == 0x3b && funct3 == 0x00 && funct7 == 0x20;
    sll   = opcode == 0x33 && funct3 == 0x01 && funct7 == 0x00;
    sllw  = opcode == 0x3b && funct3 == 0x01 && funct7 == 0x00;
    slt   = opcode == 0x33 && funct3 == 0x02 && funct7 == 0x00;
    sltu  = opcode == 0x33 && funct3 == 0x03 && funct7 == 0x00;
    xor   = opcode == 0x33 && funct3 == 0x04 && funct7 == 0x00;
    srl   = opcode == 0x33 && funct3 == 0x05 && funct7 == 0x00;
    srlw  = opcode == 0x3b && funct3 == 0x05 && funct7 == 0x00;
    sra   = opcode == 0x33 && funct3 == 0x05 && funct7 == 0x20;
    sraw  = opcode == 0x3b && funct3 == 0x05 && funct7 == 0x20;
    or    = opcode == 0x33 && funct3 == 0x06 && funct7 == 0x00;
    and   = opcode == 0x33 && funct3 == 0x07 && funct7 == 0x00;
    mul   = opcode == 0x33 && funct3 == 0x00 && funct7 == 01;
    mulh  = opcode == 0x33 && funct3 == 0x01 && funct7 == 01;
    mulhsu= opcode == 0x33 && funct3 == 0x02 && funct7 == 01;
    mulhu = opcode == 0x33 && funct3 == 0x03 && funct7 == 01;
    div   = opcode == 0x33 && funct3 == 0x04 && funct7 == 01;
    divu  = opcode == 0x33 && funct3 == 0x05 && funct7 == 01;
    rem   = opcode == 0x33 && funct3 == 0x06 && funct7 == 01;
    remu  = opcode == 0x33 && funct3 == 0x07 && funct7 == 01;
    csrrc = opcode == 0x73 && funct3 == 0x3;
    csrrs = opcode == 0x73 && funct3 == 0x2;
    csrrw = opcode == 0x73 && funct3 == 0x1;
    csrrci= opcode == 0x73 && funct3 == 0x7;
    csrrsi= opcode == 0x73 && funct3 == 0x6;
    csrrwi= opcode == 0x73 && funct3 == 0x5;

    ret.is_branch = beq | bne | blt | bge | bltu | bgeu;
    ret.is_lsu = lb | lh | lw | ld | lbu | lhu | lwu | sb | sh | sw | sd;
    ret.is_load = lb | lh | lw | ld | lwu | lbu | lhu;
    ret.is_store = sb | sh | sw | sd;
    ret.is_csr = csrrc | csrrs | csrrw | csrrci | csrrsi | csrrwi;
    ret.csr_type = csrrc ? CSRRC :
                   csrrs ? CSRRS :
                   csrrw ? CSRRW :
                   csrrci ? CSRRCI :
                   csrrsi ? CSRRSI :
                   csrrwi ? CSRRWI : 0xff;
    ret.lsu_type = sb ? SB :
                   sh ? SH :
                   sw ? SW :
                   sd ? SD :
                   lb ? LB :
                   lh ? LH :
                   lw ? LW :
                   ld ? LD :
                   lbu ? LBU :
                   lhu ? LHU :
                   lwu ? LWU : 0xff;
    ret.is_alu = lui | auipc | addi | slti | sltiu | xori | ori | andi | slli |
                 srli | srai | add | sub | sll | slt | sltu | xor | srl | sra |
                 or | and | addiw | slliw | srliw | sraiw | addw | subw | sllw |
                 srlw | sraw;
    ret.is_mdu = mul | mulh | mulhu | mulhsu | div | divu | rem | remu;
    ret.alu_type = lui ? LUI :
                   auipc ? AUIPC :
                   (addi | add) ? ADD :
                   (addiw | addw) ? ADDW :
                   (slti | slt) ? SLT :
                   (sltu | sltiu) ? SLTU :
                   (xori | xor) ? XOR :
                   (ori | or) ? OR :
                   (andi | and) ? AND :
                   (slli | sll) ? SLL :
                   (slliw | sllw) ? SLLW :
                   (srli | srl) ? SRL :
                   (srliw | srlw) ? SRLW :
                   (srai | sra) ? SRA :
                   (sraiw | sraw) ? SRAW :
                   subw ? SUBW :
                   sub ? SUB : 0xff;
    ret.mdu_type = mul ? MUL :
                   mulh ? MULH :
                   mulhu ? MULHU :
                   mulhsu ? MULHSU :
                   div ? DIV :
                   divu ? DIVU :
                   rem ? REM :
                   remu ? REMU : 0xff;
    bool shift_imm = slli | srli | srai | slliw | srliw | sraiw;
    bool type_i = jalr | lb | lh | lw | ld | lbu | lhu | lwu |
      addi | slti | sltiu | xori | ori | andi | addiw | csrrc | csrrs | csrrw | csrrci | csrrsi | csrrwi;
    bool type_s = sb | sh | sw | sd;
    bool type_b = beq | bne | blt | bge | bltu | bgeu;
    int branch_type = beq ? BEQ :
                      bne ? BNE :
                      blt ? BLT :
                      bge ? BGE :
                      bltu ? BLTU :
                      bgeu ? BGEU :
                      jal ? JAL :
                      jalr ? JALR : 0xff;
    ret.branch_type = branch_type;
    bool type_u = lui | auipc;
    bool type_j = jal;
    bool type_r = slli | srli | srai |
      slliw | srliw | sraiw | addw | subw | sllw | srlw | sraw |
      add | sub | sll| slt | sltu | xor | srl |
      sra | or | and;

    if (type_i) ret.instr_type = TYPE_I;
    else if (type_s) ret.instr_type = TYPE_S;
    else if (type_b) ret.instr_type = TYPE_B;
    else if (type_u) ret.instr_type = TYPE_U;
    else if (type_j) ret.instr_type = TYPE_J;
    else if (type_r) ret.instr_type = TYPE_R;


    ret.imm = type_i ? imm_expansion(imm_i, 11):
              type_s ? imm_expansion(imm_s, 11):
              type_b ? imm_expansion(imm_b, 12):
              type_u ? imm_expansion(imm_u, 31) :
              type_r ? rs2 :
              type_j ? imm_expansion(imm_j, 20): 0;
    ret.rs1 = rs1;
    ret.rs2 = rs2;
    ret.rd = rd;
    ret.shift_imm = shift_imm;
    // ret.pc is handled outside
    return ret;
}

// FIXME: decode must decide if there are enough rename space
// i.e. if we have more branch than expected, only send some of them to rename
void ID_step() {
    if (jmp_to_is_sig[0].redirect_valid) {
        printf("ID redirect handling\n");
        // flush the entire queue
        decode_queue_start = 0;
        decode_queue_end = 0;
        queue_size = 0;
        global_instr_idx = jmp_to_is_sig[0].instr_idx + 1;
        id_to_rn_sig[1].valid = false;
        id_to_if_sig[1].allow_in = false;
    } else {
        // Instruction decode
        if (!(decode_queue_end == decode_queue_start) && rn_to_id_sig[0].allow_in) {
            // instructions in queue and we are allowed to enter rename
            int dequeue_size = rn_to_id_sig[0].rename_size;
            #ifdef DEBUG
            printf("ID: dequeue size %d\n", dequeue_size);
            #endif // DEBUG 
            for (int i = 0; i < dequeue_size; ++i) {
                uint32_t instr = decode_queue[(decode_queue_start + i) % DECODE_QUEUE_SIZE];
                id_to_rn_sig[1].decoded[i] = decode(instr);
                id_to_rn_sig[1].decoded[i].pc = decode_pc[(decode_queue_start + i) % DECODE_QUEUE_SIZE];
                id_to_rn_sig[1].decoded[i].instr_idx = global_instr_idx++;
                #ifdef DEBUG
                printf("instr: idx %llu, pc 0x%x\n", id_to_rn_sig[1].decoded[i].instr_idx, id_to_rn_sig[1].decoded[i].pc);
                #endif // DEBUG
            }
            id_to_rn_sig[1].valid = true;
            id_to_rn_sig[1].decode_size = dequeue_size;

            decode_queue_start = (decode_queue_start + dequeue_size) % DECODE_QUEUE_SIZE;
            queue_size -= dequeue_size;
        } else {
            id_to_rn_sig[1].valid = false;
        }
        // actually not perfect emu, since RTL queue cannot push after pop
        if (!(decode_queue_end == decode_queue_start - 1) && if_to_id_sig[0].valid) {
            // push some more instrs
            // instruction size should always fit in our queue size
            // assert((decode_queue_start + if_to_id_sig[0].instr_size) % DECODE_QUEUE_SIZE < decode_queue_end);
            #ifdef DEBUG
            printf("ID enqueue size %d\n", if_to_id_sig[0].instr_size);
            #endif // DEBUG
            for (int i = 0; i < if_to_id_sig[0].instr_size; ++i) {
                decode_queue[(decode_queue_end + i) % DECODE_QUEUE_SIZE] = if_to_id_sig[0].instr[i];
                decode_pc[(decode_queue_end + i) % DECODE_QUEUE_SIZE] = if_to_id_sig[0].fetch_pc + i * 4;
            }
            decode_queue_end = (decode_queue_end + if_to_id_sig[0].instr_size) % DECODE_QUEUE_SIZE;
            queue_size += if_to_id_sig[0].instr_size;
        }
    }
    

    
    if (!(decode_queue_end == decode_queue_start - 1)) {
        #ifdef DEBUG
        printf("ID allowin size %d\n", id_to_if_sig[1].instr_allow_size);
        #endif // DEBUG
        id_to_if_sig[1].allow_in = true;
        id_to_if_sig[1].instr_allow_size = DECODE_QUEUE_SIZE - queue_size > 4 ? 4 : DECODE_QUEUE_SIZE - queue_size;
    }
}