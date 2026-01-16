// This program just demonstrates how CWD state is global
// to the process (not thread local). When ran depending
// on thread scheduling, the result might be / or /tmp.

#include <linux/limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

void *thread_func(void *thread_id) {
  long id = (long)thread_id;

  switch (id) {
  case 1:
    if (chdir("/tmp") != 0) {
      exit(EXIT_FAILURE);
    }
    printf("%ld w: %s\n", id, "/tmp");
    break;
  default:
    if (chdir("/") != 0) {
      exit(EXIT_FAILURE);
    }
    printf("%ld w: %s\n", id, "/");
    break;
  }

  char buf[PATH_MAX + 1];
  if (syscall(SYS_getcwd, buf, PATH_MAX + 1) < 0) {
    perror("getcwd syscall failed in thread");
    exit(EXIT_FAILURE);
  }

  printf("%ld r: %s\n", id, buf);

  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t t1, t2;

  if (pthread_create(&t1, NULL, thread_func, (void *)1L) != 0) {
    perror("t1 failed to create");
    return EXIT_FAILURE;
  }

  if (pthread_create(&t2, NULL, thread_func, (void *)2L) != 0) {
    perror("t2 failed to create");
    return EXIT_FAILURE;
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  char buf[PATH_MAX + 1];
  if (syscall(SYS_getcwd, buf, PATH_MAX + 1) < 0) {
    perror("getcwd syscall failed");
    return EXIT_FAILURE;
  }

  printf("final read: %s\n", buf);

  return EXIT_SUCCESS;
}
