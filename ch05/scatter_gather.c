#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct my_iovec {
  void *iov_base;
  size_t iov_len;
};

ssize_t my_readv(int fd, const struct my_iovec *iov, int iovcnt) {
  ssize_t n = 0;

  for (int i = 0; i < iovcnt; i++) {
    size_t to_read = iov[i].iov_len;
    char *buf = iov[i].iov_base;

    while (to_read > 0) {
      ssize_t ni = read(fd, buf, to_read);
      if (ni < 0) {
        if (n == 0) {
          return ni;
        }
        return n;
      }
      if (ni == 0) {
        return n;
      }

      buf += ni;
      to_read -= ni;
      n += ni;
    }
  }

  return n;
}

ssize_t my_writev(int fd, const struct my_iovec *iov, int iovcnt) {
  ssize_t n = 0;

  for (int i = 0; i < iovcnt; i++) {
    size_t to_write = iov[i].iov_len;
    const char *buf = iov[i].iov_base;

    while (to_write > 0) {
      ssize_t ni = write(fd, buf, to_write);
      if (ni < 0) {
        if (n == 0) {
          return ni;
        }
        return n;
      }

      buf += ni;
      to_write -= ni;
      n += ni;
    }
  }

  return n;
}

int main(int argc, char *argv[]) {
  const char *filename = "/tmp/scatter_gather_test_file.bin";
  int fd;

  // Example data to write
  char *buf1 = "Hello, ";
  char *buf2 = "world!";
  char *buf3 = " This is my_readv/my_writev test.";

  struct my_iovec iov_write[3];
  iov_write[0].iov_base = buf1;
  iov_write[0].iov_len = strlen(buf1);
  iov_write[1].iov_base = buf2;
  iov_write[1].iov_len = strlen(buf2);
  iov_write[2].iov_base = buf3;
  iov_write[2].iov_len = strlen(buf3);

  // Open file for writing
  fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open for write");
    exit(1);
  }

  ssize_t nw = my_writev(fd, iov_write, 3);
  printf("my_writev wrote %zd bytes\n", nw);
  close(fd);

  // Prepare buffers to read into
  char rbuf1[10] = {0};
  char rbuf2[10] = {0};
  char rbuf3[100] = {0};

  struct my_iovec iov_read[3];
  iov_read[0].iov_base = rbuf1;
  iov_read[0].iov_len = sizeof(rbuf1) - 1;
  iov_read[1].iov_base = rbuf2;
  iov_read[1].iov_len = sizeof(rbuf2) - 1;
  iov_read[2].iov_base = rbuf3;
  iov_read[2].iov_len = sizeof(rbuf3) - 1;

  // Open file for reading
  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    perror("open for read");
    exit(1);
  }

  ssize_t nr = my_readv(fd, iov_read, 3);
  printf("my_readv read %zd bytes\n", nr);
  close(fd);

  // Print what was read
  printf("Buffer1: '%s'\n", rbuf1);
  printf("Buffer2: '%s'\n", rbuf2);
  printf("Buffer3: '%s'\n", rbuf3);

  return EXIT_SUCCESS;
}
