#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "./helpers.inc"

int main(int argc, char *argv[]) {
  assert(argc == 3);

  int n_blocks = atoi(argv[1]);
  int block_size = atoi(argv[2]);

  assert(n_blocks > 0);
  assert(block_size > 0);

  // allocate a block buffer upfront for sending and receiving
  char *buf = malloc(block_size);
  assert(buf);
  memset(buf, 0xAA, block_size);

  // create UNIX domain socket pair
  int fd[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == 0);

  struct timespec start, end;

  // fork
  pid_t pid = fork();
  assert(pid >= 0);

  if (pid == 0) {
    // ---- child: writer ----
    close(fd[0]); // close reader end

    for (int i = 0; i < n_blocks; i++) {
      ssize_t off = 0;
      while (off < block_size) {
        ssize_t n = write(fd[1], buf + off, block_size - off);
        if (n < 0) {
          perror("write");
          _exit(EXIT_FAILURE);
        }
        off += n;
      }
    }

    close(fd[1]);
    _exit(EXIT_SUCCESS);
  }

  // ---- parent: reader ----
  close(fd[1]); // close writer end

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < n_blocks; i++) {
    ssize_t off = 0;
    while (off < block_size) {
      ssize_t n = read(fd[0], buf + off, block_size - off);
      if (n < 0) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      if (n == 0) {
        fprintf(stderr, "unexpected EOF\n");
        exit(EXIT_FAILURE);
      }
      off += n;
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  close(fd[0]);

  int status;
  waitpid(pid, &status, 0);
  assert(WIFEXITED(status));

  double elapsed = timespec_diff_sec(start, end);
  double bytes = (double)n_blocks * block_size;
  double bandwidth = bytes / elapsed;

  printf("elapsed: %.6f s\n", elapsed);
  printf("bytes:   %.0f\n", bytes);
  printf("bandwidth: %.2f bytes/s (%.2f MB/s)\n", bandwidth,
         bandwidth / (1024.0 * 1024.0));

  return EXIT_SUCCESS;
}
