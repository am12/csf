#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <iostream>

using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::string;
using std::stoi;


/** Store arguments into variables
 * Parameters:
 * 
 * 
 * Return:
 * 1 for error
 * 0 for not 
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
    if ((blocks < 4) || (!pow2_check(blocks)) || (!pow2_check(sets))) {
        return 1;
    }
    //if write b and no write lal were both specified 
    if ((write_b) && !(write_a)) {
        return 1;
    }
    return 0;
}


bool pow2_check(int check) {
    if (check > 0) {
    // continously divide i if it is even
    while (check % 2 == 0) {
      check = check / 2;
    }
    // check if n is a power of 2
    if (check == 1) {
      return true;
    } else {
      return false;
    }
  }
}