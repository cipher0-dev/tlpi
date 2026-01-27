#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_fn(void *val) {
  setenv("FOO", val, 1);
  printf("Thread sees FOO=%s\n", getenv("FOO"));
  return NULL;
}

int main() {
  setenv("FOO", "hello", 1);

  pthread_t t1, t2;

  pthread_create(&t1, NULL, thread_fn, "test1");
  pthread_create(&t2, NULL, thread_fn, "test2");

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("\nmain sees FOO=%s\n", getenv("FOO"));
}
