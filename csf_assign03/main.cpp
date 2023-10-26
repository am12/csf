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

int main(int argc, char **argv) {
    if (argc != 7) {
        cerr << ("Error: wrong number of arguments") << endl;
        return 1;
    }
    int sets, blocks, bytes;
    bool write_a, write_b, lru;
    int err_args = 0;
    try {
        err_args = store_args(sets, blocks, bytes, write_a, write_b, lru, argv);
    }
    catch (...) {
        cerr << ("Error: Invalid arguments") << endl;
        return 2;
    }
    if (err_args == 1) {
        cerr << ("Error: Invalid arguments") << endl;
        return 2;
    }

    //write_a false -> no write 
    //write_b false -> write back
    //lru false -> fifo

    //n sets of 1 block each: direct mapped 
    //n sets of m blocks each: m-way set-associative
    //1 set of n blocks: fully associative 

    

    return 0;
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
void print_output(int store_hits, int store_misses, int load_hits, int load_misses, int total_cycles) {
    cout << "Total loads: " << load_hits + load_misses << endl;
    cout << "Total stores: " << store_hits + store_misses << endl;
    cout << "Load hits: " << load_hits << endl;
    cout << "Load misses: " << load_misses << endl;
    cout << "Store hits: " << store_hits << endl;
    cout << "Store misses: " << store_misses << endl;
    cout << "Total cycles: " << total_cycles << endl;
}