#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>

struct Slot {
	uint32_t tag;
	bool valid;
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