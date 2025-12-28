
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: %s <file>\n",argv[0]);
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  printf("file %s was opened for writing, pausing...\n", argv[0]);
  pause();

  if (close(fd) < 0) {
    perror("close");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
