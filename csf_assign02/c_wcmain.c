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


void load_table(FILE *file, struct WordEntry **word_table, uint32_t *total_words);
void get_stats(struct WordEntry **word_table, uint32_t *unique_words, const unsigned char **best_word, uint32_t *best_word_count);


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

  // define hash table of WordEntry elements and load the table
  struct WordEntry *word_table[HASHTABLE_SIZE];
  load_table(file, word_table, &total_words);
  
  // get and print statistics
  get_stats(word_table, &unique_words, &best_word, &best_word_count);
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

  return 0; // ran without error
}

/* 
 * Loads the words of given file into the hash table of words, keeping track of total count
 *
 * Parameters: 
 *    file : pointer to file where input is contained
 *    word_table : the hash table of words
 *    total_words : total number of words read in
 * 
*/
void load_table(FILE *file, struct WordEntry **word_table, uint32_t *total_words) {
  // variable for a word to be read in
  unsigned char word[MAX_WORDLEN + 1];

  // initialize the word hash table to null values
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    word_table[i] = NULL;
  }

  // main body to enter words into hash table
  while (wc_readnext(file, word)) {
    (*total_words)++; // increment total number of words
    wc_tolower(word); // convert to lowercase
    wc_trim_non_alpha(word); // trim word
    struct WordEntry *entry = wc_dict_find_or_insert(word_table, HASHTABLE_SIZE, word); // find or insert word in hash table
    ++entry->count; // increment WordEntry count
  }
}

/* 
 * Loads the words of given file into the hash table of words, keeping track of total count
 *
 * Parameters: 
 *    word_table : the hash table of words
 *    unique_words : total number of unique words found
 *    best_word : the word that occurs most frequently
 *    best_word_count : the count of the most frequent word
 * 
*/
void get_stats(struct WordEntry **word_table, uint32_t *unique_words, const unsigned char **best_word, uint32_t *best_word_count) {
  // gather summary statistics
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *current = word_table[i];
    while (current != NULL) {
      (*unique_words)++;
      if (current->count > *best_word_count || (current->count == *best_word_count && wc_str_compare(current->word, *best_word) < 0)) { // check for highest occurring word
        *best_word_count = current->count;
        *best_word = current->word;
      }
      current = current->next;
    } 
  }
}