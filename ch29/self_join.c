#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *thread_f(void *arg) {
  printf("thread: thread running, attempting join...\n");
  pthread_join(pthread_self(), NULL);
  printf("thread: thread was joined!\n");
  return NULL;
}

int main() {
  pthread_t t;
  int s;

  printf("main: creating thread\n");
  s = pthread_create(&t, NULL, thread_f, NULL);
  if (s != 0) {
    printf("pthread_create: error: %d\n", s);
  }

  printf("main: thread created, sleeping\n");
  sleep(2);

  return EXIT_SUCCESS;
}
