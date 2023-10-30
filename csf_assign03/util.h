#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	int64_t state; // valid = 0, dirty = 1, default = -1
	uint32_t load_ts, access_ts;

	// constructor
	Slot(uint32_t tag, int state) : tag(tag), state(state), load_ts(0), access_ts(0) {}
};

struct Set {
	std::vector<Slot> slots;
	uint32_t set_size, block_size; // # blocks/set, # bytes/block

	// constructor
	Set(uint32_t set_size_, uint32_t block_size_) {
		set_size = set_size_;
		block_size = block_size_;
		for (uint32_t i = 0; i < set_size; i++) {
    		slots.emplace_back(0, -1);
		}
	}
};

struct Cache {
	uint32_t cache_size; // # sets/cache
	std::vector<Set> sets;
	int total_ts;

	// constructor
	Cache(uint32_t cache_size_, uint32_t set_size, uint32_t block_size) {
		cache_size = cache_size_;
		total_ts = 0;
		for (uint32_t i = 0; i < cache_size; i++) {
    		sets.emplace_back(set_size, block_size);
		}
	}
};

struct Summary {
	int cycles, l_hits, l_misses, s_hits, s_misses;

	// constructor
	Summary() : cycles(0), l_hits(0), l_misses(0), s_hits(0), s_misses(0) {}
};


int store_args(int &sets, int &blocks, int &bytes, bool &write_a, bool &write_b, bool &lru, char **argv);

bool is_power_2(int n);

int log2(int n);

int dec_to_bin(int n);

unsigned hex_to_int(std::string addr);

unsigned index_mask(int n);

int get_index(unsigned tag, Set set);

int process_line(std::string line, Cache &cache, bool write_a, bool write_t, bool lru, int index_length, int offset_length, int bytes, int &total_cycles);

int store(unsigned index, unsigned tag, Cache &cache, bool write_a, bool write_b, bool lru, int bytes, int &cycles);

int load(unsigned index, unsigned tag, Cache &cache, bool lru, int bytes, int &cycles);

#endif //UTIL_H
