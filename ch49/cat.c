#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  assert(argc == 2);
  char *path = argv[1];

  // open fd
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  // find out the size of the file
  struct stat statbuf;
  if (fstat(fd, &statbuf) < 0) {
    perror("fstat");
    return EXIT_FAILURE;
  }

  // NOTE: The file could easily be above the memory limits of the system.
  off_t size = statbuf.st_size;
  char *addr = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

  // read all the bytes, writing them to stdout
  while (size > 0) {
    ssize_t n = write(STDOUT_FILENO, addr, size);
    if (n < 0) {
      if (n == EINTR) {
        continue;
      }
      perror("write");
      return EXIT_FAILURE;
    }

    size -= n;
  }

  return EXIT_SUCCESS;
}
