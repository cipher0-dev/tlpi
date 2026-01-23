#include <stdio.h>
#include <stdlib.h>

char *FOO = "init";

char **foo() { return &FOO; }

int main() {
  *foo() = "hello";
  printf("value: %s\n", FOO);
  // NOTE: this prints hello because function calls can be used as lvalues, TIL
  return EXIT_SUCCESS;
}
