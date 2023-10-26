#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	int mem;
	bool valid, dirty; //true when written to; true when dirty bit
	uint32_t load_ts, access_ts;

	// constructor
	Slot(bool valid, bool dirty, int mem) : valid(valid), dirty(dirty), mem(mem), load_ts(0), access_ts(0) {}
};

struct Set {
	std::vector<Slot> slots;
	uint32_t set_size, block_size;

	// constructor
	Set(uint32_t set_size, uint32_t block_size) {
		set_size = set_size;
		block_size = block_size;
		//add slots based on set_size, do not map yet
		for(int i = 0; i < set_size; i++) {
			Slot s = Slot(false, false, 0);
				slots.push_back(s);
		}
	}
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
