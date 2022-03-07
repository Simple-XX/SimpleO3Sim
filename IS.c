#include "IS.h"
extern struct cmt_wakeup_info cmt_wakeup_sig[2];

extern struct rn_to_is rn_to_is_sig[2];
struct is_to_rn is_to_rn_sig[2];

struct is_to_ex is_to_ex_sig[2];
extern struct ex_to_is ex_to_is_sig[2];

struct ScoreBoard {
    // all renamed
    int rs1, rs2;
    bool rs1_ready, rs2_ready;

    // for storage of rename and decode info
    struct decode_info decoded;
    struct rename_info renamed;
    
    // temp tag for squash the story board
    bool issued;
}scoreboard[ISSUE_QUEUE_SIZE];

int scoreboard_size = 0;

bool ready_to_launch(struct decode_info *decoded, struct rename_info *renamed) {
    // decide if we are free to issue
}

void IS_step() {
    int issue_size;
    // instruction issue
    if (rn_to_is_sig[0].valid) {
        assert(scoreboard_size + rn_to_is_sig[0].issue_size <= ISSUE_QUEUE_SIZE);
        issue_size = rn_to_is_sig[0].issue_size;
        for (int i = 0; i < issue_size; ++i) {
            scoreboard[scoreboard_size++].rs1 = rn_to_is_sig[0].renamed[i].rs1_phy;
            scoreboard[scoreboard_size++].rs2 = rn_to_is_sig[0].renamed[i].rs2_phy;
            scoreboard[scoreboard_size++].rs1_ready = rn_to_is_sig[0].renamed[i].rs1_ready;
            scoreboard[scoreboard_size++].rs2_ready = rn_to_is_sig[0].renamed[i].rs2_ready;

            scoreboard[scoreboard_size++].decoded = rn_to_is_sig[0].decoded[i];
            scoreboard[scoreboard_size++].renamed = rn_to_is_sig[0].renamed[i];
        }
        scoreboard_size += issue_size;
    }
    if (cmt_wakeup_sig[0].valid) {
        // accept any possible commit wakeup
        for (int i = 0; i < scoreboard_size; ++i) {
            for (int j = 0; j < cmt_wakeup_sig[0].commit_size; ++j) {
                if (scoreboard[i].rs1 == cmt_wakeup_sig[0].committed[j].recycle_dst) {
                    scoreboard[i].rs1_ready = true;
                }
                if (scoreboard[i].rs2 == cmt_wakeup_sig[0].committed[j].recycle_dst) {
                    scoreboard[i].rs2_ready = true;
                }
            }
            
        }
    }
    

    // decide what to issue
    //is_to_ex_sig[1].alu_size;
    int issued_count = 0;
    for (int i = 0; i < scoreboard_size; ++i) {
        if (ready_to_launch(&scoreboard[i].decoded, &scoreboard[i].renamed)) {
            scoreboard[i].issued = true;
            // add to ex channel
            --scoreboard_size;
            is_to_ex_sig[1].alu[issued_count].decoded = scoreboard[i].decoded;
            is_to_ex_sig[1].alu[issued_count++].renamed = scoreboard[i].renamed;
            if (issued_count == ISSUE_SIZE) break;
        }
    }

    // squash scoreboard
    int lookup_ptr = 0, append_ptr = 0;
    for (; append_ptr < ISSUE_QUEUE_SIZE; ++append_ptr) {
        if (!scoreboard[append_ptr].issued) break;
    }
    while (lookup_ptr < ISSUE_QUEUE_SIZE) {
        if (!scoreboard[lookup_ptr].issued) {
            // if not issued, copy to the first free position
            scoreboard[append_ptr] = scoreboard[lookup_ptr];
            for (++append_ptr; append_ptr < ISSUE_QUEUE_SIZE; ++append_ptr) {
                if (!scoreboard[append_ptr].issued) break;
            }
        }
        ++lookup_ptr;
    }
}