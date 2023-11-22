#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** 
 * Function that compares two values together 
 * 
 * Parameters:
 *  a - first value
 *  b - second value
 * 
 * Returns:
 *  -1 if first value is less than second 
 *  1 if second is less than first
 *  0 if equal
**/ 
int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *) left_;
  int64_t right = *(int64_t *) right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

/**
 * Sorts a sequence of int64_t elements in ascending order using quicksort
 *
 * Parameters:
 *  arr - pointer to the array of int64_t elements.
 *  begin - the starting index of the subarray to be sorted (inclusive).
 *  end - the ending index of the subarray to be sorted (exclusive).
 */
void seq_sort(int64_t *arr, size_t begin, size_t end) {
  qsort(arr + begin, end - begin, sizeof(int64_t), compare_i64);
}

/**
 * Merges two sorted halves of an array into a single sorted array.
 * 
 * Parameters:
 *  arr - the input array containing the elements to be merged.
 *  begin - the starting index of the left subarray.
 *  mid - the ending index of the left subarray and the starting index of the right subarray.
 *  end - the ending index of the right subarray.
 *  temparr - temporary array to store the merged elements.
*/
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) {
      break;
    }
    if (at_end_l) {
      *dst++ = *right++;
    } else if (at_end_r) {
      *dst++ = *left++;
    } else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0) {
        *dst++ = *left++;
      } else {
        *dst++ = *right++;
      }
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

/**
 * Prints an error message to the standard error stream and exits the program with a status code of 1.
 *
 * Parameters:
 *  msg - a pointer to a null-terminated string containing the error message to be displayed.
 */
void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(EXIT_FAILURE);
}

/**
 * Performs parallel merge sort on the given array within the specified range.
 * Parameters:
 *  arr - pointer to the array to be sorted.
 *  begin - starting index of the range to be sorted (inclusive).
 *  end - ending index of the range to be sorted (exclusive).
 *  threshold - the threshold at which the sorting switches to a sequential method.
 */
void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;

  if (size <= threshold) { // problem is small enough to solve sequentially
    seq_sort(arr, begin, end);
    return; 
  } else { // greater than threshold, recursively call merge with child processes

    // divide the list and sort each half in parallel
    size_t mid = begin + (size/2);

    pid_t pid1 = fork(); // create first child process
    if (pid1 == -1) { 
      fatal("fork 1 failed"); 
    } else if (pid1 == 0) {
      merge_sort(arr, begin, mid, threshold); 
      exit(EXIT_SUCCESS);
    }

    pid_t pid2 = fork(); // create second child process
    if (pid2 == -1) {
      fatal("fork 2 failed");
    } else if (pid2 == 0) {
      merge_sort(arr, mid, end, threshold); 
      exit(EXIT_SUCCESS); 
    }

    // wait for child processes to complete
    int wstatus1, wstatus2;
    pid_t pid_real1 = waitpid(pid1, &wstatus1, 0); 
    pid_t pid_real2 = waitpid(pid2, &wstatus2, 0); 

    if (pid_real1 == -1) {
      fatal("waitpid 1 failed!"); 
      exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(wstatus1)) {
      fatal("subprocess 1 crashed, was interrupted, or did not exit normally");
      exit(EXIT_FAILURE);
    }
    if (WEXITSTATUS(wstatus1) != 0) {
      fatal("subprocess 1 returned a non-zero exit code"); 
      exit(EXIT_FAILURE);
    }

    if (pid_real2 == -1) {
      fatal("waitpid 2 failed!"); 
      exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(wstatus2)) {
      fatal("subprocess 2 crashed, was interrupted, or did not exit normally");
      exit(EXIT_FAILURE);
    }
    if (WEXITSTATUS(wstatus2) != 0) {
      fatal("subprocess 2 returned a non-zero exit code"); 
      exit(EXIT_FAILURE);
    }

    // allocate temp array now, so we can avoid unnecessary work if the malloc fails
    int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
    if (temp_arr == NULL) {
      fatal("malloc() of temp array failed!");
      exit(EXIT_FAILURE);
    }

    // child processes completed successfully, so in theory we should be able to merge their results
    merge(arr, begin, mid, end, temp_arr);

    // copy data back to main array
    for (size_t i = 0; i < size; i++) {
      arr[begin + i] = temp_arr[i];
    }

    // now we can free the temp array
    free(temp_arr);

    // success!
  }
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    // report an error (threshold value is invalid)
    fatal("threshold value is invalid\n");
  }

 // open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    // report an error (file cannot be opened)
    fatal("file couldn't be opened\n");
  }
 
  // use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    // report an error (fstat error)
    fatal("fstat failed on file\n");
  }

  size_t file_size_in_bytes = statbuf.st_size;
  
  // map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fatal("bad map\n");
  }
  
  // sort the data!
  uint64_t num_el = file_size_in_bytes/sizeof(int64_t);
  merge_sort(data, 0, num_el, threshold);

  // unmap and close the file
  int um = munmap(data, file_size_in_bytes); 
  if (um != 0) {
    fatal("could not unmap file\n");
  }
  
  // exit with a 0 exit code if sort was successful
  int c = close(fd);
  if (c != 0) {
    fatal("could not close file\n");
  }
  
  // exit with a 0 exit code if sort was successful
  exit(EXIT_SUCCESS);
}
