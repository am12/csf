#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	bool valid, dirty;
	//int64_t state; // valid = 0, dirty = 1, default = -1
	uint32_t load_ts, access_ts; // FIFO timestamp; LRU timestamp

	// constructor
	Slot(uint32_t tag) : tag(tag), valid(false), dirty(false), load_ts(0), access_ts(0) {}
};

struct Set {
	std::vector<Slot> slots;
	uint32_t set_size, block_size; // # blocks/set, # bytes/block

	// constructor
	Set(uint32_t num_blocks, uint32_t num_bytes) : set_size(num_blocks), block_size(num_bytes) {
		for (uint32_t i = 0; i < set_size; i++) {
    		slots.emplace_back(0);
		}
	}
};

struct Cache {
	uint32_t cache_size; // # sets/cache
	std::vector<Set> sets;
	int load_counter, total_ts; // FIFO counter, LRU timestamp

	// constructor
	Cache(uint32_t num_sets, uint32_t set_size, uint32_t block_size) : cache_size(num_sets), load_counter(0), total_ts(0) {
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

int get_index(Set *set, unsigned tag);

int process_line(std::string line, Cache &cache, bool write_a, bool write_t, bool lru, int index_length, int offset_length, int bytes, int &total_cycles);

int store(Set *set, unsigned tag, Cache &cache, bool write_a, bool write_b, bool lru, int bytes, int &cycles);

int load(Set *set, unsigned tag, Cache &cache, bool lru, int bytes, int &cycles);

#endif //UTIL_H
