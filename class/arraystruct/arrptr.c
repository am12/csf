#include <stdio.h>

int main(void) {
  int arr[3] = { 1, 2, 3 };
  printf("%p\n%p\n%p\n", &arr[0], &arr[1], &arr[2]);
  return 0;
}
