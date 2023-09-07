#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  result.data[0] = val; // least significant bit is a uint32_t
  for(int i = 1; i < 8; i++) { // all other bits are 0
    result.data[i] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for(int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
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

// Takes 8-character hex string and converts to unsigned long.
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

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = (char *) malloc(sizeof(char) * 9); // starting size
  char *buf = hex;
  int resize;
  
  for (int i=7; i>=0; i--) {
    uint32_t cur = val.data[i];
    buf[8] = '\0'; // MAY DELETE
    sprintf(buf, "%08x", cur); // format with leading 0s
    if (i>0) {
      resize = 8 * (9-i) + 1;
      hex = (char *) realloc(hex, sizeof(char) * resize);
    }
    buf = buf + 8;
  }

  // DEBUG
  printf("\n");
  for (int i=0; i<resize; i++) {
    printf("%c", hex[i]);
  }

  // sprintf(buf, "%x", val);   // format without leading 0s
  // sprintf(buf, "%08x", val); // format with leading 0s

  // ensure that the outputted hex string has no leading 0s
  char *start = hex;
  int finalSize = resize;
  while (*start == '0' && start < buf-1) {
    finalSize--;
    start++;
  }
  memmove(hex, start, finalSize);
  printf("\nALI%d", finalSize);
  hex = (char *) realloc(hex, sizeof(char) * finalSize);

  // DEBUG
  for (int i=0; i<finalSize; i++) {
    printf("%c", hex[i]);
  }

  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
