#include <stdio.h>

struct Player {
  int x, y;
  char symbol;
  short health;
};

int main(void) {
  printf("%lu\n", sizeof(struct Player));
  return 0;
}
