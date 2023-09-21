#include "interface.h"
#include "clint.h"
#include "common.h"

struct lsu_to_clint lsu_to_clint_sig;
struct clint_to_lsu clint_to_lsu_sig;
struct clint_interrupt clint_interrupt_sig[2];

// non-standard width but convenient
static uint64_t msip, mtime, mtimecmp;

void clint_step() {
    mtime += 1;
    if (mtime == mtimecmp) {
        // timer interrupt
        clint_interrupt_sig[1].valid = true;
    }
    // foo implementation here
    if (lsu_to_clint_sig.valid) {
        assert(lsu_to_clint_sig.size < 3);
        if (lsu_to_clint_sig.write) {
            switch (lsu_to_clint_sig.addr) {
                case MSIP_OFF:
                    msip = lsu_to_clint_sig.data & 1;
                    break;
                case MTIME_OFF:
                    mtime = lsu_to_clint_sig.data;
                    break;
                case MTIMECMP_OFF:
                    mtimecmp = lsu_to_clint_sig.data;
                    break;
                default:
                    assert(1);
                    break;
            }
        } else {
            clint_to_lsu_sig.valid = true;
            switch (lsu_to_clint_sig.addr) {
            case MSIP_OFF:
                clint_to_lsu_sig.rdata = msip;
                break;
            case MTIME_OFF:
                clint_to_lsu_sig.rdata = mtime;
            case MTIMECMP_OFF:
                clint_to_lsu_sig.rdata = mtimecmp;
            default:
                assert(1);
                break;
            }
        }
    }
}