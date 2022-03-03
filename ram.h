#ifndef RAM_H
#define RAM_H

/*
 * this file contents all external functions for ram
*/
extern struct ram_req iram_req, dram_req;
extern struct ram_resp iram_resp, dram_resp;

void init_ram();

void dram_step();
void iram_step();

#endif