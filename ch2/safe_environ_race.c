#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_fn(void *val) {
  pthread_mutex_lock(&env_mutex);
  setenv("FOO", val, 1);
  printf("Thread sees FOO=%s\n", getenv("FOO"));
  pthread_mutex_unlock(&env_mutex);
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
