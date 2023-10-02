#include <stdio.h>
#include <inttypes.h>

uint32_t sum_elts(uint32_t arr[], unsigned len) {
  uint32_t *p = arr, *end = arr + len;
  uint32_t sum = 0;
  while (p < end) {
    sum += *p;
    p++;
  }
  return sum;
}

int main(void) {
  uint32_t vals[10];
  printf("Enter 10 integer values: ");
  for (unsigned i = 0; i < 10; i++) {
    scanf("%" SCNu32, &vals[i]);
  }
  uint32_t sum = sum_elts(vals, 10);
  printf("Sum is %" PRIu32 "\n", sum);
  return 0;
}
