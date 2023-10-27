#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <iostream>

using namespace std;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::string;

// helper function declarations
void print_summary(int store_hits, int store_misses, int load_hits, int load_misses, int total_cycles);
void print_cache(int sets, int blocks, int bytes, bool write_a, bool write_b, bool lru);

int main(int argc, char **argv) {
    // example input: ./csim 256 4 16 write-allocate write-back lru < sometracefile

    // error handling
    if (argc != 7) {
        cerr << ("Error: Wrong number of arguments") << endl;
        return 1;
    }

    int sets, blocks, bytes;
    bool write_alloc, write_back, lru;
    int err_args = 0;

    try {
        err_args = store_args(sets, blocks, bytes, write_alloc, write_back, lru, argv);
    }
    catch (...) {
        cerr << ("Error: Invalid arguments") << endl;
        return 2;
    }
    if (err_args == 1) {
        cerr << ("Error: Invalid arguments") << endl;
        return 2;
    }

    // DEBUG
    print_cache(sets, blocks, bytes, write_alloc, write_back, lru);

    // generate index, blocks, offset lengths
    int index_l = log2(sets), block_l = log2(blocks), offset_l = log2(bytes);

    if (index_l < 0 || block_l < 0 || offset_l < 2) {
        cerr << "Error: Cache params not valid" << endl;
        return 3;
    }

    /////////
    Cache cache(sets, blocks, bytes);
    Summary summary;
    string command;
    while (getline(cin, command)) {
        int write_status = process_line(command, cache, write_alloc, write_back, lru, index_l, offset_l, bytes, summary.cycles);
        if (write_status < 0) {
            return 4;
        } else if (command[0] == 's' && write_status) {
            summary.s_hits++;
        } else if (command[0] == 's' && !write_status) {
            summary.s_misses++;
        } else if (command[0] == 'l' && write_status) {
            summary.l_hits++;
        } else if (command[0] == 'l' && !write_status) {
            summary.l_misses++;
        } else {
            cerr << "Error: Invalid command given" << endl;
            return 5;
        }
    }
    
    print_summary(summary);


    //write_a false -> no write
    //write_b false -> write back
    //lru false -> fifo
    
    //n sets of 1 block each: direct mapped 
    //n sets of m blocks each: m-way set-associative
    //1 set of n blocks: fully associative 

    // finally, print the summary statistics
    
    //print_summary();

    return 0; // return successfully
}


/**
 * Prints final counts
 * 
 * Parameters:
 * store_hits: # of time it hits when storing
 * store_misses: # of times it misses when storing
 * load_hits: # of times it hits when loading
 * load_misses: # of times it misses when loading 
 * total_cycles: total # of cycles
 * 
 * 
*/
void print_summary(Summary &s) {
    cout << "Total loads: " << s.l_hits + s.l_misses << endl;
    cout << "Total stores: " << s.s_hits + s.s_misses << endl;
    cout << "Load hits: " << s.l_hits << endl;
    cout << "Load misses: " << s.l_misses << endl;
    cout << "Store hits: " << s.s_hits << endl;
    cout << "Store misses: " << s.s_misses << endl;
    cout << "Total cycles: " << s.cycles << endl;
}

/** 
 * Prints the parameters of the cache.
 * 
*/
void print_cache(int sets, int blocks, int bytes, bool write_a, bool write_b, bool lru) {
    cout << "Number of sets: " << sets << endl; 
    cout << "Numbers of blocks: " << blocks << endl; 
    cout << "Number of bytes/block: " << bytes << endl; 
    cout << "Upon miss: " << (write_a ? "write-allocate" : "no-write-allocate") << endl;
    cout << "Upon hit: " << (write_b ? "write-back" : "write-through") << endl;
    cout << "Upon eviction: " << (lru ? "lru" : "fifo") << endl;
}