#include <stdio.h>

void echo(void) {
  char buf[4];
  gets(buf);
  puts(buf);
}

int main(void) {
  printf("Enter a line of text:\n");
  echo();
  return 0;
}
