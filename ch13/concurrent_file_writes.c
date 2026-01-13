#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s <file> <char>\n", argv[0]);
    return 1;
  }

  const char c = argv[2][0];

  int fd = open(argv[1], O_WRONLY | O_CREAT, 0644);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  char buf[4096];
  memset(buf, c, sizeof(buf));

  for (;;) {
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
      perror("lseek");
      break;
    }

    ssize_t n = write(fd, buf, sizeof(buf));
    if (n == -1) {
      perror("write");
      break;
    }

    /* optional: slow it down a bit */
    /* usleep(100000); */
  }

  close(fd);
  return 0;
}
