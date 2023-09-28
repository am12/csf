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
  while (*lhs != '\0' && *rhs != '\0') {
      if (*lhs != *rhs) {
        return *lhs - *rhs; // difference returned
      } 
      lhs++;
      rhs++;
    }

    // when one string is shorter than other 
    if (*lhs == '\0' && *rhs != '\0') {
        return -1; // lhs shorter
    } else if (*lhs != '\0' && *rhs == '\0') {
        return 1;  // rhs shorter
    }

    // strings are identical
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
  *dest = '\0'; // add null terminator back
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

  int c;
  int num = 0;

  while ((c = fgetc(in)) != EOF) {
    if (!wc_isspace(c)) {
      if (num < MAX_WORDLEN) { // within max wordlen, add char to word
        w[num] = (unsigned char) c;
        num++;
      } 
    } else { // it is whitespace
      if (num > 0) { // word was encountered
        w[num] = '\0'; 
        return 1; // success
      }
    }
  }
  if (num > 0) { // reached the max length and word found 
    w[num] = '\0'; 
    return 1; // success
  }

  return 0; // no word found
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
  int len = 0;
  if (w == NULL || *w == '\0') { // given edge cases (null value or terminator)
    return;
  }
  while (w[len] != '\0') { // find end of word
    len++;
  }
  do { // search backwards until first alpha char
    len--;
  } while (len >= 0 && !wc_isalpha(w[len]));
  if (len < 0) {
    *w = '\0'; // if all non-alpha, then return empty string
  } else {
    w[len+1] = '\0'; // terminate when found
  }
}

/* 
 * Search the specified linked list of WordEntry objects for an object containing the specified string.
 *
 * Parameters: 
 *    head : start of struct 
 *    s : string to be added 
 *    inserted : 1 if inserted, 0 if not 
 * Returns:
 *    struct pointer of the word found or inserted
*/
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  // search through bucket list for matching WordEntry
  struct WordEntry* current = head;
  while (current != NULL) {
    if (wc_str_compare(current->word, s) == 0) {
      *inserted = 0;
      
      return current; // word found, return
    }
    current = current->next;
  }
  // word not found, create new WordEntry representing word
  struct WordEntry *new = (struct WordEntry *) malloc(sizeof(struct WordEntry));
  wc_str_copy(new->word, s);
  new->count = 0;
  new->next = head; // prepend to list and return
  *inserted = 1;
  return new;
}

/* 
 * Find/insert the WordEntry object for the given string
 *
 * Parameters: 
 *    buckets : wordEntry struct array
 *    num_buckets: number of buckets
 *    s : string
 * 
 * Returns:
 *    struct pointer of the word found
 * 
*/
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  // calculate the index in the hash table and plug into find or insert function
  int hash = wc_hash(s) % num_buckets;
  int inserted;
  buckets[hash] = wc_find_or_insert(buckets[hash], s, &inserted);
  return buckets[hash];
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
