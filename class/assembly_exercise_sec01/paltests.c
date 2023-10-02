#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tctest.h"

// function prototype for the function being tested
int is_palindrome(const char *s);

typedef struct {
  const char *s1;
  const char *s2;
  const char *s3;
  const char *s4;
  const char *s5;
  const char *s6;
  const char *s7;
  const char *s8;
  const char *s9;
} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_is_palindrome_1(TestObjs *objs);
void test_is_palindrome_2(TestObjs *objs);
void test_is_palindrome_3(TestObjs *objs);
void test_is_palindrome_4(TestObjs *objs);
void test_is_palindrome_5(TestObjs *objs);
void test_is_palindrome_6(TestObjs *objs);
void test_is_palindrome_7(TestObjs *objs);
void test_is_palindrome_8(TestObjs *objs);
void test_is_palindrome_9(TestObjs *objs);

int main(int argc, char **argv) {
  // allow test function to run to be specified on the command line
  if (argc > 1)
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST(test_is_palindrome_1);
  TEST(test_is_palindrome_2);
  TEST(test_is_palindrome_3);
  TEST(test_is_palindrome_4);
  TEST(test_is_palindrome_5);
  TEST(test_is_palindrome_6);
  TEST(test_is_palindrome_7);
  TEST(test_is_palindrome_8);
  TEST(test_is_palindrome_9);

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  objs->s1 = "risetovotesir";
  objs->s2 = "adanishcustarddratsuchsinada";
  objs->s3 = "amanaplanacatacanalpanama";
  objs->s4 = "";
  objs->s5 = "notapalindrome";
  objs->s6 = "assembly is fun";
  objs->s7 = "47374";
  objs->s8 = ">>> WOW >>>";
  objs->s9 = "foo";

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_is_palindrome_1(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s1));
}

void test_is_palindrome_2(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s2));
}

void test_is_palindrome_3(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s3));
}

void test_is_palindrome_4(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s4));
}

void test_is_palindrome_5(TestObjs *objs) {
  ASSERT(0 == is_palindrome(objs->s5));
}

void test_is_palindrome_6(TestObjs *objs) {
  ASSERT(0 == is_palindrome(objs->s6));
}

void test_is_palindrome_7(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s7));
}

void test_is_palindrome_8(TestObjs *objs) {
  ASSERT(1 == is_palindrome(objs->s8));
}

void test_is_palindrome_9(TestObjs *objs) {
  ASSERT(0 == is_palindrome(objs->s9));
}
