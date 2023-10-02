#include <stdio.h>

struct Treasure {
  long amount;
  int type;
};

int main(void) {
  printf("%u\n", (unsigned)
    sizeof(struct Treasure));
  return 0;
}
