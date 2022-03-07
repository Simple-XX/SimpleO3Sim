#include "fakeCMT.h"

extern struct cmt_wakeup_info cmt_wakeup_sig[2];

// bypass from ex
extern struct is_to_ex is_to_ex_sig[2];
extern struct ex_to_is ex_to_is_sig[2];

void fakeCMT_step() {
    // this is only for debug purpose
    int commit_count = 0;
    if (is_to_ex_sig[0].valid && is_to_ex_sig[0].alu_size) {
        cmt_wakeup_sig[1].valid = true;
        // only consider alu port
        for (int i = 0; i < is_to_ex_sig[0].alu_size; ++i) {
            // just commit
            cmt_wakeup_sig[1].committed[commit_count++].recycle_dst = is_to_ex_sig[0].alu[i].renamed.rd_phy.b;
        }
        cmt_wakeup_sig[1].commit_size = commit_count;
    }
}