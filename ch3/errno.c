#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  ssize_t cnt = read(-1, NULL, 10);
  if (cnt == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
