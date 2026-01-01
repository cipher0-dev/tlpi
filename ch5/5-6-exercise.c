#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char *file = argv[1];
  int fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  int fd2 = dup(fd1);
  int fd3 = open(file, O_RDWR);
  write(fd1, "Hello,", 6); // Hello,
  write(fd2, " world", 6); // Hello, world
  lseek(fd2, 0, SEEK_SET);
  write(fd1, "HELLO,", 6); // HELLO, world
  write(fd3, "Gidday", 6); // Gidday world
  return EXIT_SUCCESS;
}
