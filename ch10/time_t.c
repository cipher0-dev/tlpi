#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
  printf("sizeof(time_t) = %zu bytes\n", sizeof(time_t));
  printf("sizeof(time_t) = %zu bytes\n", sizeof(suseconds_t));
  return EXIT_SUCCESS;
}
