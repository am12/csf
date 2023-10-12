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

char pload[] = "Total loads: %d\n";
char pstores[] = "Total stores: %d\n";
char phits[] = "Load hits: %d\n";
char pmisses[] = "Load misses: %d\n";
char pshits[] = "Store hits: %d\n";
char psmisses[] = "Store misses: %d\n";
char pcycles[] = "Total cycles: %d\n";

int main(int argc, char **argv) {
    if (argc != 7) {
        cerr << ("Error: wrong number of arguments");
        return 1;
    }
    int sets, blocks, bytes;
    bool write_a, write_b, lru;
    int err_args = 0;
    try {
        err_args = store_args(sets, blocks, bytes, write_a, write_b, lru, argv);
    }
    catch (...) {
        cerr << ("Error: Invalid arguments");
        return 2;
    }
    if (err_args == 1) {
        cerr << ("Error: Invalid arguments");
        return 2;
    }


    return 0;
}