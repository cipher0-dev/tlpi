#define _GNU_SOURCE
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int signo) { printf("handler invoked: %d\n", signo); }

void *thread_func(void *x) {
  pid_t tid = gettid();

  printf("thread: kill -USR1 %ld\n", (long)tid);

  printf("pausing thread: %d\n", tid);
  pause();
  printf("unpaused thread: %d\n", tid);
  return NULL;
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  pthread_t t1, t2;

  if (pthread_create(&t1, NULL, thread_func, NULL) != 0) {
    perror("t1 failed to create");
    return EXIT_FAILURE;
  }

  if (pthread_create(&t2, NULL, thread_func, NULL) != 0) {
    perror("t2 failed to create");
    return EXIT_FAILURE;
  }

  printf("main: kill -USR1 %ld\n", (long)getpid());

  // Mask USR1 for main thread and the signal to the main thread and see if one
  // of the other threads handles it and gets unpaused.
  // - Indeed it does. Additionally, if you send a signal to a thread that has
  // it
  //   masked, the main thread can also be interrupted to handle it if it isn't
  //   masked.

  // sigset_t set;
  // sigemptyset(&set);
  // sigaddset(&set, SIGUSR1);
  //
  // if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
  //   perror("pthread_sigmask");
  //   return EXIT_FAILURE;
  // }

  printf("pausing main thread\n");
  pause();
  printf("unpaused main thread\n");

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  return EXIT_SUCCESS;
}
