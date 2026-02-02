#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/uds_echo.sock"
#define BUF_SIZE 4096

static void die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(void) {
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1)
    die("socket");

  struct sockaddr_un addr = {
      .sun_family = AF_UNIX,
  };
  strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    die("connect");

  char buf[BUF_SIZE];
  ssize_t n;

  // Simple loop: read from stdin, send to socket, read echo, print
  while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    ssize_t off = 0;

    // write all bytes to socket
    while (off < n) {
      ssize_t w = write(fd, buf + off, n - off);
      if (w <= 0)
        die("write");
      off += w;
    }

    // read echoed data back
    off = 0;
    while (off < n) {
      ssize_t r = read(fd, buf + off, n - off);
      if (r < 0)
        die("read");
      if (r == 0)
        goto done; // server closed
      off += r;
    }

    // write to stdout
    off = 0;
    while (off < n) {
      ssize_t w = write(STDOUT_FILENO, buf + off, n - off);
      if (w <= 0)
        die("write stdout");
      off += w;
    }
  }

done:
  close(fd);
  return 0;
}
