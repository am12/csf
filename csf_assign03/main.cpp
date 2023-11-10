#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <iostream>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

// helper function declaration
void print_summary(Summary &s);

int main(int argc, char **argv) {

    // check input args
    if (argc != 7) {
        cerr << ("Error: Wrong number of arguments") << endl;
        return 1;
    }

    // read in args
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

    // generate index, blocks, offset lengths
    int index_l = log2(sets), block_l = log2(blocks), offset_l = log2(bytes);
    if (index_l < 0 || block_l < 0 || offset_l < 2) {
        cerr << "Error: Cache params not valid" << endl;
        return 3;
    }

    // create cache and summary objects
    Cache cache(sets, blocks, bytes);
    Summary summary;

    // parse the input and execute commands
    string command;
    while (getline(cin, command)) {
        char c = command[0];
        int write_status = process_line(command, cache, write_alloc, write_back, lru, index_l, offset_l, bytes, summary.cycles);
        if (write_status < 0) {
            cerr << "Error: Bad write status" << endl;
            return 4;
        } else if (c == 's' && write_status) {
            summary.s_hits++;
        } else if (c == 's' && !write_status) {
            summary.s_misses++;
        } else if (c == 'l' && write_status) {
            summary.l_hits++;
        } else if (c == 'l' && !write_status) {
            summary.l_misses++;
        } else {
            cerr << "Error: Invalid command given" << endl;
            return 5;
        }
    }

    // print statistics
    print_summary(summary);

    return 0; // return successfully
}


/**
 * Prints final counts.
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