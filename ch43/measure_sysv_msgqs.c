#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "./helpers.inc"

struct msgbuf {
  long mtype;
  char mtext[]; // flexible array member
};

int main(int argc, char *argv[]) {
  assert(argc == 3);

  int n_blocks = atoi(argv[1]);
  int block_size = atoi(argv[2]);

  assert(n_blocks > 0);
  assert(block_size > 0);

  long msgmax = 0;
  {
    int fd = open("/proc/sys/kernel/msgmax", O_RDONLY);
    assert(fd > 2);

    char buf[32] = {0};
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    close(fd);
    assert(n >= 0);

    msgmax = strtol(buf, NULL, 10);
  }
  if (block_size > msgmax) {
    fprintf(stderr,
            "Error: block_size (%d) is larger than system MSGMAX (%ld).\n",
            block_size, msgmax);
    return EXIT_FAILURE;
  }

  /* allocate a block buffer upfront for sending and receiving */
  char *buf = malloc(block_size);
  assert(buf);
  memset(buf, 0xAA, block_size);

  /* create message queue */
  int msqid = msgget(IPC_PRIVATE, 0600);
  assert(msqid != -1);

  /* allocate the message once */
  struct msgbuf *msg = malloc(sizeof(struct msgbuf) + block_size);
  assert(msg);
  msg->mtype = 1;
  memcpy(msg->mtext, buf, block_size);

  struct timespec start, end;

  pid_t pid = fork();
  assert(pid >= 0);

  if (pid == 0) {
    /* child: send messages */
    for (int i = 0; i < n_blocks; i++) {
      if (msgsnd(msqid, msg, block_size, 0) == -1) {
        perror("msgsnd");
        _exit(EXIT_FAILURE);
      }
    }
    _exit(EXIT_SUCCESS);
  }

  /* parent: receive messages */
  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int i = 0; i < n_blocks; i++) {
    if (msgrcv(msqid, buf, block_size, 0, 0) == -1) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  /* cleanup */
  waitpid(pid, NULL, 0);
  msgctl(msqid, IPC_RMID, NULL);

  double elapsed = timespec_diff_sec(start, end);
  double bytes = (double)n_blocks * block_size;
  double bandwidth = bytes / elapsed;

  printf("elapsed: %.6f s\n", elapsed);
  printf("bytes:   %.0f\n", bytes);
  printf("bandwidth: %.2f bytes/s (%.2f MB/s)\n", bandwidth,
         bandwidth / (1024.0 * 1024.0));

  return EXIT_SUCCESS;
}
