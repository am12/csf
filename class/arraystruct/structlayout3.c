#include <stdio.h>

#define OFFSET_OF(s,f) \
((unsigned) ((char*)&s.f - (char*)&s))

struct Player {
  int x, y;
  char symbol;
  short health;
};

int main(void) {
  struct Player p;
  printf("x offset=%u\n", OFFSET_OF(p,x));
  printf("y offset=%u\n", OFFSET_OF(p,y));
  printf("symbol offset=%u\n", OFFSET_OF(p,symbol));
  printf("health offset=%u\n", OFFSET_OF(p,health));
  return 0;
}
