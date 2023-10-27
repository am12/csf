#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <sstream>
#include <iostream>

using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::string;
using std::stoi;
using std::hex;
using std::stoul;
using std::exception;
using std::stringstream;


/** 
 * Store arguments into variables
 * 
 * Parameters:
 * 
 * Return:
 * 1 for error with arguments
 * 0 for no errors
*/
int store_args(int &sets, int &blocks, int &bytes, bool &write_a, bool &write_b, bool &lru, char **argv) {

    //storing into vars
    sets = stoi(argv[1]);
    blocks = stoi(argv[2]);
    bytes = stoi(argv[3]);
    write_a = (strcmp(argv[4], "write-allocate") == 0);
    write_b = (strcmp(argv[5], "write-back") == 0);
    lru = (strcmp(argv[6], "lru") == 0);

    //check invalid paramters
    if ((bytes < 4) || (!is_power_2(blocks)) || (!is_power_2(sets)) || (!is_power_2(bytes))) {
        return 1; // error
    }

    // can't be write-back with no-write-allocate
    if ((write_b) && !(write_a)) {
        return 1; // error
    }

    return 0; // args are fine
}

/**
 * Checks if a decimal number is a power of 2
 * 
 * Parameters:
 * n (int) - given decimal number
 * 
 * Returns:
 * true if number is a power of 2, else false
*/
bool is_power_2(int n) {
    if (n <= 0) {  
        return false;
    }
    return (n & (n - 1)) == 0;
}


/**
 * Gets the log base 2 of a number, corresponding to length of a binary string;
 * 
 * Parameters:
 * 
 * Returns:
 * the log base 2 of n
 * -1 if not a valid number
 * 
*/
int log2(int n) {
    if (!is_power_2(n)) {
        return -1;
    }

    int count = 0;
    while (n > 1) {
        n /= 2;
        count++;
    }
    return count;
}


/**
 * Converts a decimal number to binary
 *
 * Parameters:
 * n (int) - decimal number
 *
 * Returns:
 *   number in binary
 *   -1 if n is not a power of 2
 */
int dec_to_bin(int n) {
  int count = 0;
  while (n > 1) {
      if (!is_power_2(n)) {
          return -1;
      }
      n /= 2;
      count++;
  }
  return count;
}

/*
 * Converts a 32-bit hex address to unsigned int.
 *
 * Parameters:
 *   addr - address in 0xffffffff format
 *
 * Returns:
 *   address as an unsigned int
 *   -1 if invalid address
 */
unsigned hex_to_int(string addr) {
    unsigned int result = -1;
    try {
        result = stoul(addr, nullptr, 16);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return result;
}

/**
 * Creates index mask 
 * 
 * Parameters:
 * n - length of mask
 * 
 * Returns:
 * index
*/
unsigned index_mask(int n) {
    unsigned int mask = 0;
    for (int i = 0; i < n; ++i) {
        mask *= 2;
        mask += 1;
    }
    return mask;
}

/*
 * Processes one line of input (contains store/load, address).
 *
 * Parameters:
 *   line - the line to be processed in string form
 *   cache - reference to the cache
 *   write_a - 1 if write-allocate, 0 if no-write-allocate
 *   write_b - 1 if write-back, 0 if write-through
 *   lru - 1 if lru, 0 if fifo
 *   index_length - length of the index in the address 
 *   offset_length - length of the offset in the address
 *   bytes - number of bytes per block
 *   cycles - reference to the number of cycles
 *
 * Returns:
 *   1 on hit
 *   0 on miss
 *   -1 on error
 */
int process_line(string line, Cache &cache, bool write_a, bool write_b, bool lru, int index_length, int offset_length, int bytes, int &cycles) {
    char store_load = line[0]; // store or load
    unsigned addr_i;
    try {
        string address = line.substr(2, 10);
        addr_i = hex_to_int(address);
    } catch (...) {
        cerr << "Error: invalid address" << endl;
        return -1;
    }
    
    // accounts for offset
    unsigned index = (addr_i >> offset_length) & index_mask(index_length);
    unsigned tag = addr_i >> (offset_length + index_length);
    
    if (store_load == 's') {
        //store -> check if hit or miss
    } else if (store_load == 'l') {
        //load -> check if hit or miss
    } else {
        cerr << "Error: not a store or load instruction" << endl;
        return -1;
    }
    return -1;
}

/*
 * Finds the index of a block tag in a set.
 *
 * Parameters:
 *   tag - the tag of the block to be found
 *   set - the set of blocks
 *
 * Returns:
 *   index of block on hit
 *   -1 on miss
 */
int find_block(unsigned int tag, Set set) {
    for (int i = 0; i < (int) set.slots.size(); ++i) {
        if (tag == set.slots[i].tag && set.slots[i].tag != 0) {
            return i;
        }
    }
    return -1;
}

/**
 * 
*/


/**
 * 
*/
int store(unsigned int index, unsigned int tag, Cache &cache, bool write_a, bool write_b, bool lru, int bytes, int &cycles) {
    int block_index = find_block(tag, cache.sets[index]);
    int words = bytes/4;
    if (block_index != -1) { // hit
        if (write_b) {
            // write only to cache, mark dirty
            cycles += 1;
            cache.sets[index].slots[block_index].valid = -1;
        } else {
            // writes to cache and memory
            cycles += 100;
        }
        if (lru) {
            //TODO: rotate back 

        }
        return 1;
    }
    // miss
    if (write_a) {
        // write from memory to cache
        cycles += 100 * words;

        // write to cache

        //TODO: rotatebackS w index -1

        cache.sets[index].slots[0].tag = tag;
        cache.sets[index].slots[0].valid = 1;
        if (write_b) {
            // write to cache
            cycles += 1;
        } else {
            // write to cache and memory
            cycles += 100; 
        } 
    } else {
        if (!write_b) {
            // write to memory
            cycles += 100;
        }       
    }
    return 0;
}

/**
 * 
 * 
*/
int load(unsigned int index, unsigned int tag, Cache &cache, bool lru, int bytes, int &cycles) {
    int block_index = find_block(tag, cache.sets[index]);
    int words = bytes / 4;
    if (block_index != -1) {
        // load hit
        cycles += 1;
        if (lru) {
            //TODO:rotate back
        }
        return 1;
    }
    // load miss
    cycles += 100 * words;
    //TODO:rotate back with -1 index
    //if dirty
    if (!cache.sets[index].slots[0].valid) {
        cycles += 100 * words;
    }
    cache.sets[index].slots[0].tag = tag;
    cache.sets[index].slots[0].valid = 1;
    cycles += 1;
    return 0;
}


