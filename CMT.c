#include "CMT.h"

extern struct ex_to_cmt ex_to_cmt_sig[2];
struct ex_to_cmt ex_to_cmt_sig[2];

struct cmt_wakeup_info cmt_wakeup_sig[2];

uint64_t commit_head;



void CMT_step() {
    // time to commit
}