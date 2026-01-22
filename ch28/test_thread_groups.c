#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static int child_fn(void *arg) {
  printf("child: running (pid=%d)\n", getpid());
  sleep(40);
  printf("child: exiting\n");
  return 0;
}

int main(void) {
  printf("main: running (pid=%d)\n", getpid());
  void *stack = malloc(STACK_SIZE);
  if (!stack) {
    perror("malloc");
    exit(1);
  }

  int flags = CLONE_FILES | CLONE_FS | CLONE_SIGHAND | CLONE_VM | CLONE_THREAD |
              CLONE_PARENT | SIGCHLD;

  pid_t pid = clone(child_fn, stack + STACK_SIZE, flags, NULL);
  if (pid == -1) {
    perror("clone");
    exit(1);
  }

  sleep(20);
  printf("main: exiting\n");
  return 0;
}
