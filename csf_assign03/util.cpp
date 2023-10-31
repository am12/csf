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
    unsigned index = addr_i >> offset_l, tag = addr_i >> (offset_l + index_l);
    index &= index_mask(index_l);  

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
        if (tag == set->slots[i].tag && set->slots[i].state != -1) {
            return i; // cache hit
        }
    }
    return -1; // cache miss
}


/**
 * Store at the given address.
*/
int store(Set *set, unsigned tag, Cache &cache, bool write_a, bool write_b, bool lru, int bytes, int &cycles) {
    // retrieve index for cache (check if hit or miss)
    int set_index = get_index(set, tag);
    if (set_index >= 0) { // hit
        // update the timestamp of LRU to global cycle number
        Slot *sl = &(set->slots[set_index]);
        sl->access_ts = cache.total_ts;
        if (write_b) { // write-back -> write to cache only, mark dirty
            cycles++;
            sl->state = 1;
        } else { // write-through -> write to cache and memory
            cycles += 100;
        }
        cache.total_ts++; // update timestamp of cache
        return 1;
    } else { // miss
        if (write_a) { // write-allocate -> load value from memory 
            load(set, tag, cache, lru, bytes, cycles);
            if (write_b) { // write-back -> write value to cache
                set_index = get_index(set, tag);
                Slot *s = &(set->slots[set_index]);
                s->state = 1;
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
    }
    cache.total_ts++;
    return 0;
}

/**
 * Load from the given address.
*/
int load(Set *set, unsigned tag, Cache &cache, bool lru, int bytes, int &cycles) {
    int set_index = -1;
    int words = bytes / 4;
    // check if block is full
    for (int i = 0; i < (int) set->slots.size(); i++) {
        Slot *s = &(set->slots[i]);
        if (tag == s->tag && s->state != -1) {
            set_index = i;
            break;
        }
        if (s->state == -1) {
            s->state = 0; // valid
            s->tag = tag;                
            s->access_ts = cache.total_ts; //lru timestamp
            cycles++;
            cache.total_ts++;
            cycles += 100 *words;
            return 0;
        }
    }
    if (set_index >= 0) { // load hit
        // update the timestamp of LRU to global cycle number
        Slot *sl = &(set->slots[set_index]);
        sl->access_ts = cache.total_ts;
        cycles++;
        cache.total_ts++;
        return 1;
    } else { // load miss and block is full = evict ASSUMING LRU
        cycles += 100 * words;
        unsigned cur_cycles = cycles;
        int lru_index = -1;
        for(int j = 0; j < (int) set->slots.size(); j++){
            Slot *ss = &(set->slots[j]);
            if (cur_cycles > ss->access_ts){
                cur_cycles = ss->access_ts;
                lru_index = j; 
            }
        } 
        if (set->slots[lru_index].state == 1) {
            cycles += 100 * words; 
        }
        Slot *s = &(set->slots[lru_index]);
        s->state = -1;
        // load for real
        for (int j=0; j < (int) set->slots.size(); j++) {
            Slot *s = &(set->slots[j]);
            if (s->state == -1) {
                s->state = 0; // valid
                s->tag = tag;
                s->access_ts = cache.total_ts; //lru timestamp
            }
        }
        cycles++;
    }
    cache.total_ts++;
    return 0;
}


