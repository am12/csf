#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	bool valid, dirty; //true when written to; true when dirty bit
	uint32_t load_ts, access_ts;
};

struct Set {
	std::vector<Slot> slots;
};

struct Cache {
	std::vector<Set> sets;
};


int store_args(int &sets, int &blocks, int &bytes, bool &write_a, bool &write_b, bool &lru, char **argv);
bool is_power_2(int n);
int dec_to_bin(int n);
unsigned int hex_to_int(string addr);
unsigned int index_mask(int n);
int process_line(string line, Cache &cache, bool write_a, bool write_t, bool lru, int index_length, int offset_length, int bytes, int &total_cycles);

#endif //UTIL_H
