#include <assert.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  // Create POSIX message queue
  const char *mqname = "/measure_mq";
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = block_size;
  attr.mq_curmsgs = 0;

  long msgsize_max = 0;
  {
    int fd = open("/proc/sys/fs/mqueue/msgsize_max", O_RDONLY);
    assert(fd > 2);

    char buf[32] = {0};
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    assert(n >= 0);

    msgsize_max = strtol(buf, NULL, 10);
  }

  if (block_size > msgsize_max) {
    fprintf(stderr,
            "Error: block_size (%d) exceeds POSIX mq msgsize_max (%ld).\n",
            block_size, msgsize_max);
    return EXIT_FAILURE;
  }

  mq_unlink(mqname);
  mqd_t mq = mq_open(mqname, O_CREAT | O_EXCL | O_RDWR, 0600, &attr);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    return EXIT_FAILURE;
  }

  // validate block_size is not too large
  struct mq_attr actual_attr;
  mq_getattr(mq, &actual_attr);
  if (block_size > actual_attr.mq_msgsize) {
    fprintf(stderr, "Error: block_size (%d) exceeds mq_msgsize (%ld)\n",
            block_size, actual_attr.mq_msgsize);
    mq_close(mq);
    mq_unlink(mqname);
    return EXIT_FAILURE;
  }

  struct timespec start, end;

  pid_t pid = fork();
  assert(pid >= 0);

  if (pid == 0) {
    // ---- child: writer ----
    for (int i = 0; i < n_blocks; i++) {
      if (mq_send(mq, buf, block_size, 0) == -1) {
        perror("mq_send");
        _exit(EXIT_FAILURE);
      }
    }
    _exit(EXIT_SUCCESS);
  }

  // ---- parent: reader ----
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < n_blocks; i++) {
    if (mq_receive(mq, buf, block_size, NULL) == -1) {
      perror("mq_receive");
      exit(EXIT_FAILURE);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  int status;
  waitpid(pid, &status, 0);
  assert(WIFEXITED(status));

  mq_close(mq);
  mq_unlink(mqname);

  double elapsed = timespec_diff_sec(start, end);
  double bytes = (double)n_blocks * block_size;
  double bandwidth = bytes / elapsed;

  printf("elapsed: %.6f s\n", elapsed);
  printf("bytes:   %.0f\n", bytes);
  printf("bandwidth: %.2f bytes/s (%.2f MB/s)\n", bandwidth,
         bandwidth / (1024.0 * 1024.0));

  return EXIT_SUCCESS;
}
