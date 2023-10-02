#include <string.h>

int is_palindrome(const char *s) {
  size_t len = strlen(s);

  if (len < 2)
    return 1;

  const char *left = s, *right = s + len - 1;

  while (left < right) {
    if (*left != *right)
      return 0;
    ++left;
    --right;
  }

  return 1;
}
