#include <stdio.h>
#include <stdlib.h>

int a = 2;
int called(int b) {
  int c = a * b;
  printf("a: %d b: %d c: %d\n", a, b, c);
  a = 5;  
  return c;
}


int main(int argc __attribute__((unused)), char* argv[] __attribute__((unused))) {
  int b = 2, c = 3;
  printf("a: %d b: %d c: %d\n", a, b, c);
  b = called(c);
  printf("a: %d b: %d c: %d\n", a, b, c);
  return 0;
}
