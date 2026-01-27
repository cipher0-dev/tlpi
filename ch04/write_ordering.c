#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// NOTE: The writes happen out-of-order since stdio buffers output.
int main(int argc, char *argv[]) {
  int size = printf("hello");
  if (size < 0) {
    perror("printf");
    return EXIT_FAILURE;
  }

  size = write(1, "world\n", 6);
  if (size < 0) {
    perror("write");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
