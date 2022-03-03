#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"


char ram_file[PATH_LEN];

void parse_arg(int argc, char* argv[]) {
	if (argc > 1) {
		// with actual args
		// the first arg must be a mem file
		strcpy(ram_file, argv[1]);
	}
}

int main(int argc, char* argv[]) {
	parse_arg(argc, argv);
	init_ram();
	return 0;
}
