#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog) {
  fprintf(stderr, "usage: %s <file>\n", prog);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_WRONLY | O_APPEND);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  // even though we seek to offset 0, the O_APPEND here ensures that writes
  // happen to the end of the file by doing an atomic lseek(eof)+write
  off_t off = lseek(fd, 0, SEEK_SET);
  if (off != 0) {
    perror("lseek");
    return EXIT_FAILURE;
  }

  ssize_t n = write(fd, "hello\n", 6);
  if (n != 6) {
    perror("write");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
