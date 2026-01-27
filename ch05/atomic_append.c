
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog) {
  fprintf(stderr, "usage: %s <file> <num-bytes> [x]\n", prog);
}

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  int n_bytes = atoi(argv[2]);

  int flags = O_WRONLY | O_CREAT;
  bool seek;
  if (argc == 3) {
    flags |= O_APPEND;
  } else {
    seek = true;
  }

  int fd = open(argv[1], flags, 0666);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < n_bytes; i++) {
    if (seek) {
      off_t off = lseek(fd, 0, SEEK_END);
      if (off < 0) {
        perror("lseek");
        return EXIT_FAILURE;
      }
    }
    ssize_t n = write(fd, "X", 1);
    if (n != 1) {
      perror("write");
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
