/*
 * A word count program in both C and x86-64 assembly language.
 * CSF Assignment 2
 * Alan Mao, Lauren Siu
 * smao10@jhu.edu, lsiu1@jhu.edu
 */


// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdlib.h>
#include "wcfuncs.h"

/* 
 * Computes a hash code for a NUL-terminated character string
 *
 * Parameters: 
 *    *w : pointer to the start of the string
 * 
 * Returns:
 * 
 *    hash code based on the string
*/
uint32_t wc_hash(const unsigned char *w) {
  uint32_t hash = 5381;
  for (int i = 0; w[i]; i++) {
    hash = hash * 33 + w[i];
  }
  return hash;
}

/* 
 * Compares two strings lexicographically
 *
 * Parameters: 
 *    lhs : pointer to the start of the left handed string
 *    rhs : pointer to the start of the right handed string
 * 
 * Returns:
 * 
 *    -1 if lhs string is less than rhs string
 *    0 if lhs string is identical to rhs string
 *    1 if lhs string is greater than rhs string
*/
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {
  while ((wc_isspace(*lhs) == 0) || (wc_isspace(*rhs) == 0)){
    if (*lhs == *rhs) { // chars are the same
      lhs++;
      rhs++;
    } else if (*lhs == '\0' || *rhs == '\0' || *lhs != *rhs) { // chars not same, or reached end 
      return *lhs - *rhs;
    } 
  }
  return 0;
}

/* 
 * Copy NUL-terminated source string to the destination buffer.
 *
 * Parameters: 
 *    dest : pointer to the destination buffer
 *    source : pointer to the source string 
 * 
*/
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  while (*source != '\0') { // copy every character until terminator
    *dest = *source;
    dest++;
    source++;
  }
  *(++dest) = '\0'; // add null terminator back
}

/* 
 * Return 1 if the character code in c is a whitespace character
 *
 * Parameters: 
 *    c : character to determine if it is a whitespace
 * 
 * Returns:
 *    0 if c is not a whitespace
 *    1 if c is a whitespace
 * 
*/
int wc_isspace(unsigned char c) {
  switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\f':
    case '\v':
      return 1;
      break;
    default:
      return 0;
  }
}

/* 
 * Checks if character code is an alphabetic character
 *
  * Parameters: 
 *    c : character to determine if it is an alphabetic character
 * 
 * Returns:
 *    0 if c is not an alphabetic character
 *    1 if c is an alphabetic character
 * 
*/
int wc_isalpha(unsigned char c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) { 
    return 1;
  }
  return 0;
}

/* 
 * Reads next word in the input stream and stores it
 *
 * Parameters: 
 *    in : input stream
 *     w : array of strings to store the next word in
 * 
 * Returns:
 *    0 if read unsuccessful
 *    1 if read successful
*/
int wc_readnext(FILE *in, unsigned char *w) {
  int num = 0;
  int c = fgetc(in);
  while (c != EOF) {
    if (wc_isspace(c) == 0) {
      if (num < MAX_WORDLEN) {
        //within max wordlen
        w[num] = (unsigned char) c;
        num++;
      } 
    } else {
        // if white space
        if (num > 0) {
          w[num] = '\0';
          return 1;
        }
    }
    c = fgetc(in);
  }
  if (num > 0) {
    // if read
    w[num] = '\0';
    return 1;
  }
  return 0;
  //not read
}

/* 
 * Convert string to lowercase
 *
 * Parameters: 
 *    w : pointer to the string 
 * 
*/
void wc_tolower(unsigned char *w) {
  for (int i = 0; w[i]; i++) {
    if (w[i] >= 'A' && w[i] <= 'Z') {
      w[i] += 32; // moves ASCII value of uppercase char to lowercase values
    }
  }
}

/* 
 * Trim string of non-alphabetic characters
 *
 * Parameters: 
 *    w : string to trim
 * 
*/
void wc_trim_non_alpha(unsigned char *w) {
  //edge case
  if (w == NULL || *w == '\0') {
    return;
  }
  //find first null character
  while (*w != '\0') {
    w++;
  }
  //go back to the last alphabetic character
  w--;
  while (!wc_isalpha(*w)) {
    w--;
  }
  *(w + 1) = '\0';
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
/* 
 * Search the specified linked list of WordEntry objects for an object containing the specified string.
 *
 * Parameters: 
 * 
 * Returns:
 * 
*/
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  // TODO: implement
}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
/* 
 * Find/insert the WordEntry object for the given string
 *
 * Parameters: 
 * 
 * Returns:
 * 
 * 
*/
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  // TODO: implement
}

/* 
 * Free all the nodes in a given linkedlist of WordEntry objects
 *
 * Parameters: 
 *    p : pointer to the WordEntry struct
 * 
 * 
*/
void wc_free_chain(struct WordEntry *p) {
  //create pointer, free curr, go to next
  while (p != NULL) {
    struct WordEntry *next = p->next;
    free(p);
    p = next;
  }
}
