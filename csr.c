#include "csr.h"

// Not portable here: we assume that we are running on a little endian machine

struct MISA {
    // misa csr
    uint32_t isa_a:1;
    uint32_t isa_b:1;
    uint32_t isa_c:1;
    uint32_t isa_d:1;
    uint32_t isa_e:1;
    uint32_t isa_f:1;
    uint32_t isa_g:1;
    uint32_t isa_h:1;
    uint32_t isa_i:1;
    uint32_t isa_j:1;
    uint32_t isa_k:1;
    uint32_t isa_l:1;
    uint32_t isa_m:1;
    uint32_t isa_n:1;
    uint32_t isa_o:1;
    uint32_t isa_p:1;
    uint32_t isa_q:1;
    uint32_t isa_r:1;
    uint32_t isa_s:1;
    uint32_t isa_t:1;
    uint32_t isa_u:1;
    uint32_t isa_v:1;
    uint32_t isa_w:1;
    uint32_t isa_x:1;
    uint32_t isa_y:1;
    uint32_t isa_z:1;
    uint32_t zero:4;
    uint32_t mxl:2;
} misa;
struct MTVEC {
    // mtvec csr
    uint32_t pc:30;
    uint32_t mode:2;
} mtvec;
struct MTVAL {
    // mtval csr
    uint32_t pc;
} mtval;
struct MCAUSE {
    // mcause csr
    uint32_t excode:31;
    uint32_t interrupt:1;
} mcause;
struct MSCRATCH {
    // mscratch csr
    uint32_t data;
} mscratch;
struct MSTATUS {
    // mstatus csr
    uint32_t reserved_0:1;// always zero
    uint32_t SIE:1;       // always zero
    uint32_t reserved_1:1;// always zero
    uint32_t MIE:1;
    uint32_t reserved_2:1;//always zero
    uint32_t SPIE:1;// always zero
    uint32_t UBE:1; // always zero
    uint32_t MPIE:1;
    uint32_t SPP:1; // always zero
    uint32_t VS:2;  // always zero
    uint32_t MPP:2; 
    uint32_t FS:2;  // always zero
    uint32_t XS:2;  // always zero
    uint32_t MPRV:1;// always zero
    uint32_t SUM:1; // always zero
    uint32_t MXR:1; // always zero
    uint32_t TVM:1; // always zero
    uint32_t TW:1;  // always zero
    uint32_t TSR:1; // always zero
    uint32_t reserved_3:8; // always zero
    uint32_t SD:1; // always zero
} mstatus;
struct MIP {
    // mip csr
    uint32_t reserved_0:1;
    uint32_t SSIP:1;
    uint32_t reserved_1:1;
    uint32_t MSIP:1;
    uint32_t reserved_2:1;
    uint32_t STIP:1;
    uint32_t reserved_3:1;
    uint32_t MTIP:1;
    uint32_t reserved_4:1;
    uint32_t SEIP:1;
    uint32_t reserved_5:1;
    uint32_t MEIP:1;
    uint32_t reserved_6:20;
} mip;
struct MIE {
    // mie csr
    uint32_t reserved_0:1;
    uint32_t SSIE:1;
    uint32_t reserved_1:1;
    uint32_t MSIE:1;
    uint32_t reserved_2:1;
    uint32_t STIE:1;
    uint32_t reserved_3:1;
    uint32_t MTIE:1;
    uint32_t reserved_4:1;
    uint32_t SEIE:1;
    uint32_t reserved_5:1;
    uint32_t MEIE:1;
    uint32_t reserved_6:20;
} mie;
struct MEPC {
    // mepc csr
    uint32_t mepc;
} mepc;

struct MTVAL2 {
    // mtval2 csr
    uint32_t mtval2;
} mtval2;

void csr_init() {
    // initialize valid values of csrs
    misa.isa_i = 1;
    misa.isa_m = 1;
    misa.mxl = 1;

    mstatus.MPP = 3;
}

struct csr_req csr_req_sig[2];
struct csr_resp csr_resp_sig[2];

static uint32_t * get_csr(uint32_t addr) {
    switch (addr) {
        case CSR_MSTATUS:
            return &mstatus;
        case CSR_MISA:
            return &misa;
        case CSR_MIE:
            return &mie;
        case CSR_MTVEC:
            return &mtvec;
        case CSR_MSCRATCH:
            return &mscratch;
        case CSR_MEPC:
            return &mepc;
        case CSR_MCAUSE:
            return &mcause;
        case CSR_MTVAL:
            return &mtval;
        case CSR_MIP:
            return &mip;
        case CSR_MTVAL2:
            return &mtval2;
        default:
            assert(1);
            return NULL;
    }
}

void csr_step() {
    // handle csr related operations
    uint32_t *csr_src = get_csr(csr_req_sig[0].csrAddr);
    csr_resp_sig[1].rd_data = *csr_src;
    switch (csr_req_sig[0].op)
    {
    case CSRW:
        *csr_src = csr_req_sig[0].rs_data;
        break;
    case CSRS:
        *csr_src = *csr_src | csr_req_sig[0].rs_data;
        break;
    case CSRC:
        *csr_src = *csr_src ^ csr_req_sig[0].rs_data;
        break;    
    default:
        break;
    }
}