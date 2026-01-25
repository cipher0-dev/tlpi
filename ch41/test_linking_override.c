#include <stdio.h>
#include <stdlib.h>

void foo();
void bar();

int main() {
  foo();
  bar();
  printf("main: hello world!\n");
  return EXIT_SUCCESS;
}
