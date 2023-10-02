#include <stdio.h>
#include <stdint.h>

int main(void) {
  uint32_t arr[3] = { 1, 2, 3 };
  char *p = (char *) &arr[1],
       *q = (char *) &arr[0];
  uint32_t diff = (uint32_t) (p - q);
  printf("%u\n", diff);
  return 0;
}
