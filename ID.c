#include "interface.h"
#include "ID.h"

extern struct if_to_id if_to_id_sig[2];
struct id_to_if id_to_if_sig[2];

struct id_to_rn id_to_rn_sig[2];
extern struct rn_to_id rn_to_id_sig[2];

static uint32_t decode_quque[DECODE_QUEUE_SIZE];
static uint32_t decode_pc[DECODE_QUEUE_SIZE];
static int decode_queue_start = 0, decode_queue_end = 0, queue_size = 0;

static uint32_t imm_expansion(uint32_t imm, int from_bit) {
    // dup since from_bit
    uint32_t extractor = 1 << from_bit & imm;
    uint32_t ret = 0;
    for (int i = 0; i < 31 - from_bit; ++i) {
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

    bool lui, auipc, jal, jalr, beq, bne, blt, bge, bltu, bgeu;
    bool lb, lh, lw, lbu, lhu, sb, sh, sw, addi, slti, sltiu;
    bool xori, ori, andi, slli, srli, srai, add, sub, sll, slt;
    bool sltu, xor, srl, sra, or, and;

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
    sb = opcode == 0x23 && funct3 == 0x0;
    sh = opcode == 0x23 && funct3 == 0x1;
    sw = opcode == 0x23 && funct3 == 0x2;
    addi = opcode == 0x13 && funct3 == 0x0;
    slti  = opcode == 0x13 && funct3 == 0x02;
    sltiu = opcode == 0x13 && funct3 == 0x03;
    xori  = opcode == 0x13 && funct3 == 0x04;
    ori   = opcode == 0x13 && funct3 == 0x06;
    andi  = opcode == 0x13 && funct3 == 0x07;
    slli  = opcode == 0x13 && funct3 == 0x01;
    srli  = opcode == 0x13 && funct3 == 0x05 && funct7 == 0x00;
    srai  = opcode == 0x13 && funct3 == 0x05 && funct7 == 0x20;
    add   = opcode == 0x33 && funct3 == 0x00 && funct7 == 0x00;
    sub   = opcode == 0x33 && funct3 == 0x00 && funct7 == 0x20;
    sll   = opcode == 0x33 && funct3 == 0x01 && funct7 == 0x00;
    slt   = opcode == 0x33 && funct3 == 0x02 && funct7 == 0x00;
    sltu  = opcode == 0x33 && funct3 == 0x03 && funct7 == 0x00;
    xor   = opcode == 0x33 && funct3 == 0x04 && funct7 == 0x00;
    srl   = opcode == 0x33 && funct3 == 0x05 && funct7 == 0x00;
    sra   = opcode == 0x33 && funct3 == 0x05 && funct7 == 0x20;
    or    = opcode == 0x33 && funct3 == 0x06 && funct7 == 0x00;
    and   = opcode == 0x33 && funct3 == 0x07 && funct7 == 0x00;

    ret.is_branch = beq | bne | blt | bge | bltu | bgeu;
    ret.is_lsu = lb | lh | lw | lbu | lhu | sb | sh | sw;
    ret.is_load = lb | lh | lw | lbu | lhu ;
    ret.is_store = sb | sh | sw;
    ret.lsu_type = sb ? SB :
                   sh ? SH :
                   sw ? SW :
                   lb ? LB :
                   lh ? LH :
                   lw ? LW :
                   lbu ? LBU :
                   lhu ? LHU : 0xff;
    ret.is_alu = lui | auipc | addi | slti | sltiu | xori | ori | andi | slli |
                 srli | srai | add | sub | sll | slt | sltu | xor | srl | sra |
                 or | and;
    ret.alu_type = lui ? LUI :
                   auipc ? AUIPC :
                   addi | add ? ADD :
                   slti | slt ? SLT :
                   sltu | sltiu ? SLTU :
                   xori | xor ? XOR :
                   ori | or ? OR :
                   andi | and ? AND :
                   slli | sll ? SLL :
                   srli | slr ? SRL :
                   srai | sra ? SRA :
                   sub ? SUB : 0xff;
    bool type_i = jalr | lb | lh | lw | lwu | ld | lbu | lhu |
      addi | addiw | slti | sltiu | xori | ori | andi;
    bool type_s = sb | sh | sw | sd;
    bool type_b = beq | bne | blt | bge | bltu | bgeu;
    bool type_u = lui | auipc;
    bool type_j = jal;
    bool type_r = slli | srli | srai |
      add | sub | sll| slt | sltu | xor | srl |
      sra | or | and;

    ret.imm = type_i ? imm_expansion(imm_i, 11):
              type_s ? imm_expansion(imm_s, 11):
              type_b ? imm_expansion(imm_b, 12):
              type_u ? imm_u :
              type_j ? imm_expansion(imm_j, 20): 0;
    ret.rs1 = rs1;
    ret.rs2 = rs2;
    ret.rd = rd;
    // ret.pc is handled outside
    ret.imm = 
    return ret;
}

void ID_step() {
    // Instruction decode
    if (!(decode_queue_end == decode_queue_start) && rn_to_id_sig[0].allow_in) {
        // instructions in queue and we are allowed to enter rename
        int dequeue_size = rn_to_id_sig[0].rename_size;
        
        for (int i = 0; i < dequeue_size; ++i) {
            uint32_t instr = decode_queue[(decode_queue_start + i) % DECODE_QUEUE_SIZE];
            id_to_rn_sig[1].decoded[i] = decode(instr);
            id_to_rn_sig[1].decoded[i].pc = decode_pc[(decode_queue_start + i) % DECODE_QUEUE_SIZE];
        }
        id_to_rn_sig[1].valid = true;
        id_to_rn_sig[1].decode_size = dequeue_size;

        decode_queue_start = (decode_queue_start + dequeue_size) % DECODE_QUEUE_SIZE;
        queue_size -= dequeue_size;
    }
    // actually not perfect emu, since RTL queue cannot push after pop
    if (!(decode_queue_end == decode_queue_start - 1) && if_to_id_sig[0].valid) {
        // push some more instrs
        // instruction size should always fit in our queue size
        assert((decode_queue_end + if_to_id_sig[0].instr_size) % DECODE_QUEUE_SIZE < decode_queue_start);
        for (int i = 0; i < if_to_id_sig[0].instr_size; ++i) {
            decode_queue[(decode_queue_end + i) % DECODE_QUEUE_SIZE] = if_to_id_sig[0].instr[i];
            decode_pc[(decode_queue_end + i) % DECODE_QUEUE_SIZE] = if_to_id_sig[0].fetch_pc + i * 4;
        }
        decode_queue_end = (decode_queue_end + if_to_id_sig[0].instr_size) % DECODE_QUEUE_SIZE;
        queue_size += if_to_id_sig[0].instr_size;
    }
    if (!decode_queue_end == decode_queue_start - 1) {
        id_to_if_sig[0].allow_in = true;
        id_to_if_sig[0].instr_allow_size = queue_size;
    }
}