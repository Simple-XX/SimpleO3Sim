#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "interface.h"
#include "IF.h"
#include "ID.h"
#include "RN.h"
#include "IS.h"
#include "EX.h"
#include "CMT.h"
#include "fakeCMT.h"
#include "ram.h"

extern struct if_to_id if_to_id_sig[2];
extern struct id_to_if id_to_if_sig[2];
extern struct id_to_rn id_to_rn_sig[2];
extern struct rn_to_id rn_to_id_sig[2];
extern struct rn_to_is rn_to_is_sig[2];
extern struct is_to_rn is_to_rn_sig[2];
extern struct is_to_ex is_to_ex_sig[2];
extern struct ex_to_is ex_to_is_sig[2];
extern struct cmt_wakeup_info cmt_wakeup_sig[2];
extern struct is_to_ex is_to_ex_sig[2];
extern struct ex_to_is ex_to_is_sig[2];
extern struct jmp_redirectInfo jmp_to_is_sig[2];

char ram_file[PATH_LEN] = "./test.bin";

void parse_arg(int argc, char* argv[]) {
	if (argc > 1) {
		// with actual args
		// the first arg must be a mem file
		strcpy(ram_file, argv[1]);
	}
}

void move_sigs() {
	// move sigs from 1 to 0
	if_to_id_sig[0] = if_to_id_sig[1];
	id_to_if_sig[0] = id_to_if_sig[1];
	id_to_rn_sig[0] = id_to_rn_sig[1];
	rn_to_id_sig[0] = rn_to_id_sig[1];
	rn_to_is_sig[0] = rn_to_is_sig[1];
	is_to_rn_sig[0] = is_to_rn_sig[1];
	is_to_ex_sig[0] = is_to_ex_sig[1];
	ex_to_is_sig[0] = ex_to_is_sig[1];
	cmt_wakeup_sig[0] = cmt_wakeup_sig[1];
	is_to_ex_sig[0] = is_to_ex_sig[1];
	ex_to_is_sig[0] = ex_to_is_sig[1];
	ex_to_cmt_sig[0] = ex_to_cmt_sig[1];
	cmt_to_ex_sig[0] = cmt_to_ex_sig[1];
	jmp_to_is_sig[0] = jmp_to_is_sig[1];
}

int main(int argc, char* argv[]) {
	parse_arg(argc, argv);
	init_ram();

	RN_init();
	uint64_t global_cycle = 0;
	while (1) {
		#ifdef DEBUG
		printf("Cycle %llu\n", global_cycle++);
		#endif // DEBUG
		IF_step();
		ID_step();
		RN_step();
		IS_step();
		EX_step();
		CMT_step();
		move_sigs();
	}
	return 0;
}
