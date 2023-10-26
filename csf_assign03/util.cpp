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
using std::stringstream;


/** 
 * Store arguments into variables
 * 
 * Parameters:
 * 
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
    write_a = (strcmp(argv[4], "write-allocate"));
    write_b = (strcmp(argv[5], "write-back"));
    lru = (strcmp(argv[6], "lru"));

    //check invalid paramters
    if ((blocks < 4) || (!is_power_2(blocks)) || (!is_power_2(sets))) {
        return 1;
    }
    //if write_a false + write_b true: error
    if ((write_b) && !(write_a)) {
        return 1;
    }
    return 0;
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
 */
unsigned int hex_to_int(string adr) {
    std::stringstream ss;
    ss << adr.substr(2, 8);
    unsigned int out;
    ss >> std::hex >> out;
    return out;
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
unsigned int index_mask(int n) {
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
 *   write_t - 1 if write-through, 0 if write-back
 *   lru - 1 if lru, 0 if fifo
 *   index_length - length of the index in the address 
 *   offset_length - length of the offset in the address
 *   bytes - number of bytes per block
 *   total_cycles - reference to the number of cycles
 *
 * Returns:
 *   1 on hit
 *   0 on miss
 *   -1 on error
 */
int process_line(string line, Cache &cache, bool write_a, bool write_t, bool lru, int index_length, int offset_length, int bytes, int &total_cycles) {
    char store_load = line[0]; // store or load
    unsigned int addr_i;
    try {
        string address = line.substr(2, 10);
        addr_i = hex_to_int(address);
    } catch (...) {
        cerr << "Error: invalid address" << endl;
        return -1;
    }
    
    // accounts for offset
    unsigned int index = (addr_i >> offset_length) & index_mask(index_length);
    unsigned int tag = addr_i >> (offset_length + index_length);
    
    if (store_load == 's') {
        //store
    } else if (store_load == 'l') {
        //load
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
int store(unsigned int index, unsigned int tag, Cache &cache, bool write_a, bool write_t, bool lru, int bytes, int &total_cycles) {
  int block_index = find_block(tag, cache.sets[index]);
  int words = bytes/4;
  if (block_index != -1) {
      // hit
      if (write_t) {
          // writes to cache and memory
          total_cycles += 100;
      } else {
          // write only to cache, mark dirty
          total_cycles += 1;
          cache.sets[index].slots[block_index].valid = -1;
      }
      if (lru) {
          //TODO: rotate back
      }
      return 1;
  }
  // miss
  if (write_a) {
      // write from memory to cache
      total_cycles += 100 * words;

      // write to cache
      //TODO: rotatebackS w index -1
      cache.sets[index].slots[0].tag = tag;
      cache.sets[index].slots[0].valid = 1;
      if (write_t) {
          // write to cache and memory
          total_cycles += 100;
      } else {
          // write to cache
          total_cycles += 1;
    } 
  } else {
      if (write_t) {
          // write to memory
          total_cycles += 100;
      }
  }
  return 0;
}

/**
 * 
*/
int load(unsigned int index, unsigned int tag, Cache &cache, bool lru, int bytes, int &total_cycles) {
  int block_index = find_block(tag, cache.sets[index]);
  int words = bytes / 4;
  if (block_index != -1) {
      // load hit
      total_cycles += 1;
      if (lru) {
          //TODO:rotate back
      }
      return 1;
  }
  // load miss
  total_cycles += 100 * words;
  //TODO:rotate back with -1 index
  //if dirty
  if (cache.sets[index].slots[0].valid == -1) {
      total_cycles += 100 * words;
  }
  cache.sets[index].slots[0].tag = tag;
  cache.sets[index].slots[0].valid = 1;
  total_cycles += 1;
  return 0;
}


