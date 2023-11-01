#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <sstream>
#include <iostream>

using std::cerr;
using std::endl;
using std::string;
using std::stoi;
using std::stoul;
using std::exception;

/** 
 * Store arguments into variables
 * 
 * Parameters:
 * sets - num of sets to store
 * blocks - num of blocks to store
 * bytes - num of bytes to store
 * write_a - if write allocate or not
 * write_b - if write back or not
 * lru - if lru or fifo
 * argv - arguments from userS
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
 * n - integer to calculate log base 2
 * 
 * Returns:
 *   the log base 2 of n
 *   -1 if not a valid number
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

/**
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
    unsigned result = -1;
    try {
        result = stoul(addr, nullptr, 16);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return result;
}

/**
 * Creates index mask to extract the index.
 * 
 * Parameters:
 * n - length of mask
 * 
 * Returns:
 * index
*/
unsigned index_mask(int n) {
    unsigned mask = 0;
    for (int i = 0; i < n; ++i) {
        mask *= 2;
        mask += 1;
    }
    return mask;
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
int get_index(Set *set, unsigned tag) {
    for (int i = 0; i < (int) set->slots.size(); ++i) {
        if (set->slots[i].valid && tag == set->slots[i].tag) {
            return i; // cache hit
        }
    }
    return -1; // cache miss
}

/**
 * Processes one line of input (contains store/load, address).
 *
 * Parameters:
 *   line - the line to be processed in string form
 *   cache - reference to the cache
 *   write_a - 1 if write-allocate, 0 if no-write-allocate
 *   write_b - 1 if write-back, 0 if write-through
 *   lru - 1 if lru, 0 if fifo
 *   index_l - length of the index in the address 
 *   offset_l - length of the offset in the address
 *   bytes - number of bytes per block
 *   cycles - reference to the number of cycles
 *
 * Returns:
 *   1 on hit
 *   0 on miss
 *   -1 on error
 */
int process_line(string line, Cache &cache, bool write_a, bool write_b, bool lru, int index_l, int offset_l, int bytes, int &cycles) {
    char store_load = line[0]; // store or load
    unsigned addr_i;
    try {
        string address = line.substr(2, 10);
        addr_i = hex_to_int(address);
    } catch (...) {
        cerr << "Error: invalid address" << endl;
        return -1;
    }
    
    // generate the index and the tag
    unsigned index = addr_i >> offset_l;
    index &= index_mask(index_l);  
    unsigned tag = addr_i >> (offset_l + index_l);

    // get the set associated with the given index
    Set *set = &(cache.sets[index]);
    
    if (store_load == 's') { //store
        return store(set, tag, cache, write_a, write_b, lru, bytes, cycles);
    } else if (store_load == 'l') { //load 
        return load(set, tag, cache, lru, bytes, cycles);
    } else {
        cerr << "Error: not a store or load instruction" << endl;
        return -1;
    }
}


/**
 * Store at the given address.
*/
int store(Set *set, unsigned tag, Cache &cache, bool write_a, bool write_b, bool lru, int bytes, int &cycles) {
    // retrieve set index for cache
    int set_index = get_index(set, tag);

    // upon hit:
    if (set_index >= 0) {
        // update the timestamp of LRU to global cycle number
        Slot *slot = &(set->slots[set_index]);
        slot->access_ts = cache.total_ts;
        if (write_b) { // write-back -> write to cache only, mark dirty
            cycles++;
            slot->dirty = true;
        } else { // write-through -> write to cache and memory
            cycles += 100;
        }
        cache.total_ts++; // update timestamp of cache
        return 1;
    // upon miss:
    } else {
        if (write_a) { // write-allocate -> load value from memory 
            load(set, tag, cache, lru, bytes, cycles);
            if (write_b) { // write-back -> write value to cache
                set_index = get_index(set, tag);
                Slot *slot = &(set->slots[set_index]);
                slot->dirty = true;
                cycles++;
            } else { // write through -> write to cache and memory
                cycles += 100; 
            } 
        } else { // no-write-allocate
            if (!write_b) {
                // write to memory
                cycles += 100;
            }       
        }
        cache.total_ts++;
        return 0;
    }
}


void evict_block(Set *set, unsigned tag, Cache &cache, bool lru, int words, int &cycles) {
    if (lru) {
        unsigned min_cycles = cycles;
        int lru_index = -1;
        // reset all the timestamps in the set
        for (int i = 0; i < (int) set->slots.size(); i++) {
            Slot *slot = &(set->slots[i]);
            if (min_cycles > slot->access_ts) {
                min_cycles = slot->access_ts;
                lru_index = i; 
            }
        }
        Slot *lru_slot = &(set->slots[lru_index]);
        if (lru_slot->dirty) {
            cycles += 100 * words;
        }
        lru_slot->valid = false;
        lru_slot->dirty = false;
        // load for real
        for (int i = 0; i < (int) set->slots.size(); i++) {
            Slot *slot = &(set->slots[i]);
            if (!slot->valid) {
                slot->valid = true; // valid
                slot->tag = tag;
                slot->access_ts = cache.total_ts; //lru timestamp
            }
        }
    } else {
        unsigned min_counter = UINT32_MAX;
        int fifo_index = -1;
        // find the block with the lowest FIFO counter (the oldest block)
        for (int j = 0; j < (int) set->slots.size(); j++) {
            Slot *slot = &(set->slots[j]);
            if (min_counter > slot->load_ts) {
                min_counter = slot->load_ts;
                fifo_index = j;
            }
        }
        Slot *fifo_slot = &(set->slots[fifo_index]);
        if (fifo_slot->dirty) {
            cycles += 100 * words;
        }
        fifo_slot->valid = false;
        fifo_slot->dirty = false;
        // load for real
        for (int j = 0; j < (int) set->slots.size(); j++) {
            Slot *slot = &(set->slots[j]);
            if (!slot->valid) {
                slot->valid = true;
                slot->tag = tag;
                slot->access_ts = cache.total_ts; // LRU timestamp
                slot->load_ts = cache.load_counter++; // FIFO counter
            }
        }
    }
}

/**
 * Load data from the given address in the cache, handling cache misses, evictions, and replacement policies.
 * 
 * Parameters:
 *   set - pointer to the cache set from which data should be loaded
 *   tag - tag associated with the data to be loaded
 *   cache - reference to the Cache object that holds the cache structure
 *   lru - true if LRU, false if FIFO (cache replacement policy)
 *   bytes - number of bytes to be loaded from the cache
 *   cycles - number of cycles taken by this operation
 *
 * Returns:
 *   1 on cache hit (data was successfully stored in the cache)
 *   0 on cache miss (data was not found in the cache)
 */
int load(Set *set, unsigned tag, Cache &cache, bool lru, int bytes, int &cycles) {
    int words = bytes / 4;

    // search for the data first in the cache
    for (int i = 0; i < (int) set->slots.size(); i++) {
        Slot *slot = &(set->slots[i]);
        if (slot->valid && tag == slot->tag) { // cache hit -> update timestamp and return
            slot->access_ts = cache.total_ts;
            cycles++;
            cache.total_ts++;
            return 1;
        }
        if (!slot->valid) { // cache miss -> update tag and timestamps, return
            slot->tag = tag;  
            slot->valid = true;    
            slot->access_ts = cache.total_ts; // update LRU timestamp
            slot->load_ts = cache.load_counter; // update FIFO counter
            cycles++;
            cache.total_ts++;
            cache.load_counter++;
            cycles += 100 * words;
            return 0;
        }
    }

    // block is full -> evict
    cycles += 100 * words;
    evict_block(set, tag, cache, lru, words, cycles);
    cycles++;
    cache.total_ts++;
    return 0; // cache miss
    
}