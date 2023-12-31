/*
 * A simple C library implementation of a 256-bit unsigned integer data type.
 * CSF Assignment 1
 * Alan Mao, Lauren Siu
 * smao10@jhu.edu, lsiu1@jhu.edu
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"


/*
 * Create a UInt256 value from a uint32_t value. 
 * Only the least-significant 32 bits are initialized directly,
 * all other bits are set to 0
 *
 * Parameters: 
 *  val - uint32_t number
 *
 * Returns:
 *  A UInt256 variable with val in the first column
 */
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  result.data[0] = val; // least significant bit is a uint32_t
  for(int i = 1; i < 8; i++) { // all other bits are 0
    result.data[i] = 0;
  }
  return result;
}

/*
 * Create a UInt256 value from an array of uint32_t values
 * The element at index 0 is the least significant, and the element
 * at index 3 is the most significant.
 * 
 * Parameters: 
 *  data[8] - an array (size 8) of uint32_t values
 *
 * Returns:
 *  A UInt256 variable populated with values in data
 */
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for(int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

/*
 * Create a UInt256 value from a string of hexadecimal digits 
 *
 * Parameters: 
 *  *hex - pointer to a string of hexadecimal digits
 *
 * Returns:
 *  A UInt256 variable populated with converted values from hex
 */
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  int index = 0;
  const char *end = hex; // navigate to the end of the string
  while (*end != '\0') {
    end++;
  }
  while ((end > hex) || index < 8) { // get chunks of 8 hex digits (until reaching start of hex string or UInt256 is filled)
    const char *start = end-8;
    if (start < hex) { // if substring reaches before string starts, move it back to where it should start
      start = hex;
    }
    result.data[index] = hex_to_ul(start, end); // convert hex string to unsigned long
    end = start;
    index++;
  }
  while (index < 8) { // all other bits (if unfilled) get assigned 0
    result.data[index] = 0;
  }
  return result;
}

/*
 * Translates hex string to unsigned long
 *
 * Parameters: 
 *  *start - start of string
 *  *end - end of string
 *
 * Returns:
 *  A converted uint32_t value froom a hex string
 */
uint32_t hex_to_ul(const char *start, const char *end) {
  uint32_t sum = 0;
  uint32_t value;
  for (int i=0; i<(end-start); i++) {
    if (start[i] >= '0' && start[i] <= '9') {
      value = start[i] - '0';
    } else if (start[i] >= 'a' && start[i] <= 'f') {
      value = 10 + start[i] - 'a';
    } else if (start[i] >= 'A' && start[i] <= 'F') {
      value = 10 + start[i] - 'A';
    }
    sum = (sum << 4) | value; // effectively adds digits by powers of 16
  }
  return sum;
}

/*
 * Return a dynamically-allocated string of hex digits representing the
 * given UInt256 value.
 *
 * Parameters: 
 *  val - UInt256 to be translated into hex
 *
 * Returns:
 *  A hex string translated from a UInt256
 */
char *uint256_format_as_hex(UInt256 val) {
  int size = 65; // starting size
  char *hex = (char *) malloc(sizeof(char) * size);
  char *buf = hex;
  
  // incrementally add each digit to the hex string, growing the array
  for (int i=7; i>=0; i--) {
    sprintf(buf, "%08x", val.data[i]); // format with leading 0s
    buf += 8;
  }

  // ensure that the outputted hex string has no leading 0s
  int trueLen = 0; // actual length of the string without leading 0
  while (hex[trueLen] == '0' && trueLen < size-2) {
    trueLen++;
  }
  int newSize = size - trueLen;
  char *output = (char *) malloc(sizeof(char) * newSize);
  for (int i=0; i<newSize; i++) {
    output[i] = hex[trueLen + i];
  }
  free(hex);
  return output;
}

/*
 * Gets 32 bits of data from a UInt256 value.
 * Index 0 is the least significant 32 bits, index 7 is the most
 * significant 32 bits.
 *
 * Parameters: 
 *  val - the UInt256 we want to extract data from
 *  index - index of bits wanted
 *
 * Returns:
 *  A uint32_t variable extracted from a UInt256 variable
 */
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

/*
 * Computes the sum of two UInt256 values
 *
 * Parameters: 
 *  left - first value to be added
 *  right - second value to be added 
 *
 * Returns:
 *  A sum of the left and right values
 */
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  uint32_t carry = 0; // the carryover from previous addition of bits
  for (int i=0; i<8; i++) {
    uint32_t cleft = left.data[i];
    uint32_t cright = right.data[i];
    uint32_t csum = cleft + cright + carry;
    if (csum < cleft || csum < cright) { // detecting overflow
      carry = 1;
    } else {
      carry = 0; 
    }
    sum.data[i] = csum;
  }
  return sum;
}

/*
 * Computes the difference of two UInt256 values
 *
 * Parameters: 
 *  left - first value to be added
 *  right - second value to be added 
 *
 * Returns:
 *  A sum of the left and right values
 */
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  result = uint256_add(left, uint256_negate(right));
  return result;
}

/*
 * Computes the two's-complement negation of a UInt256 value
 *
 * Parameters: 
 *  val - UInt256 value we want the negated two's-complement of 
 *
 * Returns:
 *  Negated two's-complement of a value
 */
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  UInt256 one = uint256_create_from_u32(1);
  for (int i=0; i<8; i++) {
    result.data[i] = ~val.data[i];
  }
  result = uint256_add(result, one);
  return result;
}

/*
 * Return the result of rotating every bit in val nbits to
 * the left. Any bits shifted past the most significant bit
 * should be shifted back into the least significant bits.
 * 
 * Parameters:
 *  val - UInt256 value we want to rotate
 *  nbits - the number of bits we are rotating by
 * 
 * Returns: 
 *  A rotated UInt256 value
 */
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // copy all of val into result
  for (int i=0; i<8; i++) {
    result.data[i] = val.data[i];
  }
  
  // rotate bit by bit
  for (unsigned i=0; i<(nbits%256); i++) {
    uint32_t overflow = 0;
    for (int j=0; j<8; j++) {
      uint32_t previous = overflow;
      overflow = ((result.data[j] & 0x80000000U) >> 31); // save the overflowed bit
      result.data[j] = result.data[j] << 1;
      if (j != 0 && previous) {
        result.data[j] = result.data[j] + previous; // add the previous overflowed bit to end
      }
    }
    result.data[0] = result.data[0] + overflow; // add overflowed bit (last value) to the end of first value
  }
  return result;
}

/*
 * Return the result of rotating every bit in val nbits to
 * the right. Any bits shifted past the least significant bit
 * should be shifted back into the most significant bits.
 *
 * Parameters:
 *  val - UInt256 value we want to rotate
 *  nbits - the number of bits we are rotating by
 * 
 * Returns: 
 *  A rotated UInt256 value
 */
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // copy all of val into result
  for (int i=0; i<8; i++) {
    result.data[i] = val.data[i];
  }

  // rotate bit by bit
  for (unsigned i=0; i<(nbits%256); i++) {
    uint32_t overflow = 0;
    for (int j=7; j>=0; j--) {
      uint32_t previous = overflow;
      overflow = result.data[j] & 1; // save the overflowed bit
      result.data[j] = result.data[j] >> 1;
      if (j != 7 && previous) {
        result.data[j] = result.data[j] + (previous << 31); // add the previous overflowed bit to beginning
      }
    }
    result.data[7] = result.data[7] + (overflow << 31); // add overflowed bit (last value) to the beginning of first value
  }
  return result;
}