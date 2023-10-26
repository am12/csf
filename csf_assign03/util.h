#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	//1 written to, 0 not written to, -1 dirty
	int valid;
	uint32_t load_ts, access_ts;
};

struct Set {
	std::vector<Slot> slots;
};

struct Cache {
	std::vector<Set> sets;
};


#endif //UTIL_H

int store_args(int &sets, int &blocks, int &bytes, bool &write_a, bool &write_b, bool &lru, char **argv);
bool pow2_check(int check);
unsigned int hex2int(string addr);
unsigned int index_mask(int n);
int process_line(string line, Cache &cache, bool write_a, bool write_t, bool lru, int index_length, int offset_length, int bytes, int &total_cycles);

