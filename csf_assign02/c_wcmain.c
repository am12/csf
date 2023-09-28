/*
 * A word count program in both C and x86-64 assembly language.
 * CSF Assignment 2
 * Alan Mao, Lauren Siu
 * smao10@jhu.edu, lsiu1@jhu.edu
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249

// TODO: prototypes for helper functions

int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  // read file input, use stdin if file not provided
  FILE *file;
  if (argc == 2) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      fprintf(stderr, "Could not open given file\n");
      return 1;
    }
  } else if (argc == 1) {
    file = stdin;
  } else {
    fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
    return 1;
  }

  // define variables for main loop
  unsigned char word[MAX_WORDLEN + 1];
  struct WordEntry *word_table[HASHTABLE_SIZE];

  // initialize the word hash table
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    word_table[i] = NULL;
  }

  // main body to enter words into hash table
  while (wc_readnext(file, word)) {
    total_words++; // increment total number of words
    wc_tolower(word); // convert to lowercase
    wc_trim_non_alpha(word); // trim word
    struct WordEntry *entry = wc_dict_find_or_insert(word_table, HASHTABLE_SIZE, word); // find or insert word in hash table
    ++entry->count; // increment WordEntry count   
  }

  // gather summary statistics
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *current = word_table[i];
    while (current != NULL) {
      unique_words++;
      if (current->count > best_word_count || (current->count == best_word_count && wc_str_compare(word, best_word) < 0)) { // check for highest occurring word
        best_word_count = current->count;
        best_word = current->word;
      }
    } 
  }

  // print statistics
  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  // free allocated memory for word entries (cleanup)
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *current = word_table[i];
    while (current != NULL) {
      struct WordEntry *temp = current;
      current = current->next;
      free(temp);
    }
  }
  
  // make sure file is closed (if one was opened)
  if (file != stdin) {
    fclose(file);
  }

  return 0;
}

// TODO: definitions of helper functions
