#include "IS.h"

#include "RN.h"
extern struct cmt_wakeup_info cmt_wakeup_sig[2];

extern struct rn_to_is rn_to_is_sig[2];
struct is_to_rn is_to_rn_sig[2];

struct is_to_ex is_to_ex_sig[2];
extern struct ex_to_is ex_to_is_sig[2];

extern struct jmp_redirectInfo jmp_to_is_sig[2];

extern uint32_t prf[PRF_SIZE];

struct ScoreBoard {
    // all renamed
    int rs1, rs2;
    bool rs1_ready, rs2_ready;

    // for storage of rename and decode info
    struct decode_info decoded;
    struct rename_info renamed;
    
    // temp tag for squash the story board
    bool issued;
    bool valid;
} scoreboard[ISSUE_QUEUE_SIZE];

int scoreboard_size = 0;

bool ready_to_launch(struct decode_info *decoded, struct rename_info *renamed) {
    // decide if we are free to issue
    switch (decoded->instr_type) {
        case TYPE_I:
            return renamed->rs1_ready;
        case TYPE_S:
            return renamed->rs1_ready && renamed->rs2_ready;
        case TYPE_B:
            return renamed->rs1_ready && renamed->rs2_ready;
        case TYPE_U:
            return true;
        case TYPE_J:
            return true;
        case TYPE_R:
            return renamed->rs1_ready && renamed->rs2_ready;
    }
    return false;
}

void IS_step() {
    int issue_size;
    // instruction issue
    if (rn_to_is_sig[0].valid) {
        assert(scoreboard_size + rn_to_is_sig[0].issue_size <= ISSUE_QUEUE_SIZE);
        issue_size = rn_to_is_sig[0].issue_size;
        #ifdef DEBUG
        printf("IS: accept rename size %d\n", issue_size);
        printf("IS: accepted instructions:\n");
        #endif // DEBUG
        for (int i = 0; i < issue_size; ++i) {
            scoreboard[scoreboard_size + i].rs1 = rn_to_is_sig[0].renamed[i].rs1_phy;
            scoreboard[scoreboard_size + i].rs2 = rn_to_is_sig[0].renamed[i].rs2_phy;
            scoreboard[scoreboard_size + i].rs1_ready = rn_to_is_sig[0].renamed[i].rs1_ready;
            scoreboard[scoreboard_size + i].rs2_ready = rn_to_is_sig[0].renamed[i].rs2_ready;

            scoreboard[scoreboard_size + i].decoded = rn_to_is_sig[0].decoded[i];
            scoreboard[scoreboard_size + i].renamed = rn_to_is_sig[0].renamed[i];
            scoreboard[scoreboard_size + i].valid = true;
            #ifdef DEBUG
            printf("instruction pc 0x%x\n", scoreboard[scoreboard_size + i].decoded.pc);
            #endif // DEBUG
        }
        scoreboard_size += issue_size;
    }
    if (cmt_wakeup_sig[0].valid) {
        // accept any possible commit wakeup
        #ifdef DEBUG
        printf("IS: accept commit wakeup\n");
        for (int i = 0; i < cmt_wakeup_sig[0].commit_size; i++) {
            printf("recycle dst %d\n", cmt_wakeup_sig[0].committed[i].recycle_dst);
        }
        #endif // DEBUG
        for (int i = 0; i < scoreboard_size; ++i) {
            for (int j = 0; j < cmt_wakeup_sig[0].commit_size; ++j) {
                if (scoreboard[i].rs1 == cmt_wakeup_sig[0].committed[j].recycle_dst) {
                    scoreboard[i].rs1_ready = true;
                    scoreboard[i].renamed.rs1_ready = true;
                    scoreboard[i].renamed.rs1_data = prf[cmt_wakeup_sig[0].committed[j].recycle_dst];
                }
                if (scoreboard[i].rs2 == cmt_wakeup_sig[0].committed[j].recycle_dst) {
                    scoreboard[i].rs2_ready = true;
                    scoreboard[i].renamed.rs2_ready = true;
                    scoreboard[i].renamed.rs2_data = prf[cmt_wakeup_sig[0].committed[j].recycle_dst];
                }
            }
            
        }
    }
    // flush redirected ones
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i) {
        if (scoreboard[i].decoded.instr_idx > jmp_to_is_sig[0].instr_idx && jmp_to_is_sig[0].redirect_valid) {
            scoreboard[i].valid = false;
        }
    }    

    // decide what to issue
    //is_to_ex_sig[1].alu_size;
    int issued_count = 0, missed_count = 0, alu_count = 0, mdu_count = 0, jmp_count = 0;
    is_to_ex_sig[1].alu_size = 0;
    is_to_ex_sig[1].mdu_size = 0;
    is_to_ex_sig[1].jmp_size = 0;
    for (int i = 0; i < scoreboard_size; ++i) {
        if (ready_to_launch(&scoreboard[i].decoded, &scoreboard[i].renamed)) {
            #ifdef DEBUG
            printf("IS: ready to launch pc %x rs1 phy %d rs2 phy %d\n", scoreboard[i].decoded.pc, scoreboard[i].rs1, scoreboard[i].rs2);
            #endif // DEBUG
            if (!scoreboard[i].valid) {
                ++missed_count;
                continue;
            }
            scoreboard[i].issued = true;
            scoreboard[i].valid = false;
            is_to_ex_sig[1].valid = true;
            if (scoreboard[i].decoded.is_alu) {
                is_to_ex_sig[1].alu_size++;
                alu_count = is_to_ex_sig[1].alu_size;
                // add to ex channel
                is_to_ex_sig[1].alu[alu_count - 1].valid = true;
                is_to_ex_sig[1].alu[alu_count - 1].decoded = scoreboard[i].decoded;
                is_to_ex_sig[1].alu[alu_count - 1].renamed = scoreboard[i].renamed;
            } else if (scoreboard[i].decoded.is_mdu) {
                is_to_ex_sig[1].mdu_size++;
                mdu_count = is_to_ex_sig[1].mdu_size;
                // add to ex channel
                is_to_ex_sig[1].mdu[issued_count - 1].valid = true;
                is_to_ex_sig[1].mdu[issued_count - 1].decoded = scoreboard[i].decoded;
                is_to_ex_sig[1].mdu[issued_count - 1].renamed = scoreboard[i].renamed;
            } else if (scoreboard[i].decoded.instr_type == TYPE_B || scoreboard[i].decoded.instr_type == TYPE_J) {
                is_to_ex_sig[1].jmp_size++;
                jmp_count = is_to_ex_sig[1].jmp_size;
                is_to_ex_sig[1].jmp[jmp_count - 1].valid = true;
                is_to_ex_sig[1].jmp[jmp_count - 1].decoded = scoreboard[i].decoded;
                is_to_ex_sig[1].jmp[jmp_count - 1].renamed = scoreboard[i].renamed;
            }
            
            ++issued_count;
            if (issued_count == ISSUE_SIZE) break;
        }
    }
    #ifdef REG_DEBUG
    printf("real issue count %d\n", issued_count);
    #endif
    scoreboard_size -= issued_count;
    scoreboard_size -= missed_count;

    // squash scoreboard
    int lookup_ptr = 0, append_ptr = 0;
    for (; append_ptr < ISSUE_QUEUE_SIZE; ++append_ptr) {
        if (!scoreboard[append_ptr].valid) break;
    }
    lookup_ptr = append_ptr + 1;
    while (lookup_ptr < ISSUE_QUEUE_SIZE) {
        if (scoreboard[lookup_ptr].valid && !scoreboard[lookup_ptr].issued) {
            // if not issued, copy to the first free position
            scoreboard[append_ptr] = scoreboard[lookup_ptr];
            scoreboard[lookup_ptr].valid = false;
            for (++append_ptr; append_ptr < ISSUE_QUEUE_SIZE; ++append_ptr) {
                if (!scoreboard[append_ptr].valid) break;
            }
        }
        ++lookup_ptr;
    }
    #ifdef DEBUG
    printf("After squash, scoreboard:\n");
    for (int i = 0; i < ISSUE_QUEUE_SIZE; ++i) {
        if (!scoreboard[i].issued && scoreboard[i].valid)
            printf("pc %x type %d rs1 phy %d ready %d rs2 phy %d ready %d\n", scoreboard[i].decoded.pc, scoreboard[i].decoded.instr_type, scoreboard[i].rs1, scoreboard[i].rs1_ready, scoreboard[i].rs2, scoreboard[i].rs2_ready);
    }
    #endif // DEBUG
    if (scoreboard_size + RENAME_MAX > ISSUE_QUEUE_SIZE) {
        is_to_rn_sig[1].allow_in = false;
    } else {
        is_to_rn_sig[1].allow_in = true;
    }
}