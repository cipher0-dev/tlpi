#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

ssize_t write_all(int fd, const void *buf, size_t count) {
  const char *p = buf;
  size_t total = 0;

  while (total < count) {
    ssize_t n = write(fd, p + total, count - total);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }
    total += n;
  }

  return total;
}

void print_usage(const char *prog) {
  fprintf(stderr, "usage: %s [-a] <file>\n", prog);
}

int main(int argc, char *argv[]) {
  // ignore SIGPIPE so we can continue writing even if stdout
  // pipe closes early (ie: | head -n 1)
  signal(SIGPIPE, SIG_IGN);

  int opt;
  bool append = false;
  while ((opt = getopt(argc, argv, ":a")) != -1) {
    switch (opt) {
    case 'a':
      append = true;
      break;
    default:
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (optind >= argc) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }
  char *out = argv[optind];

  int flags = O_WRONLY | O_CREAT;
  if (append) {
    // NOTE: O_APPEND will seek to the EOF on every write,
    // if this isn't desired you could also just do a seek
    // once to go to the end of file before writing.
    flags |= O_APPEND;
  } else {
    flags |= O_TRUNC;
  }

  // open output file
  int fd = open(out, flags, 0666);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  bool stdout_open = true;
  char buf[BUFFER_SIZE];
  for (;;) {
    // read block from stdin
    ssize_t n_read = read(STDIN_FILENO, buf, BUFFER_SIZE);
    if (n_read < 0) {
      if (errno == EINTR) {
        continue;
      }

      close(fd);
      perror("read");
      return EXIT_FAILURE;
    } else if (n_read == 0) {
      // done reading
      break;
    }

    // write block to stdout
    if (stdout_open) {
      if (write_all(STDOUT_FILENO, buf, n_read) < 0) {
        if (errno == EPIPE) {
          // downstream closed pipe: stop writing to stdout
          stdout_open = false;
        } else {
          close(fd);
          perror("write stdout");
          return EXIT_FAILURE;
        }
      }
    }

    // write block to output file
    if (write_all(fd, buf, n_read) < 0) {
      close(fd);
      perror("write outfile");
      return EXIT_FAILURE;
    }
  }

  // close output file
  int res = close(fd);
  if (res < 0) {
    perror("close");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
