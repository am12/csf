#include <stdio.h>

struct Player {
  int x, y;
  char symbol;
  short health;
};

int main(void) {
  struct Player p;
  unsigned diff = (unsigned)
    ((char*) &p.health - (char*) &p);
  printf("%u\n", diff);
  return 0;
}
