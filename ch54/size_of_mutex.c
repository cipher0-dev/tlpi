#include <pthread.h>
#include <stdio.h>

int main() {
  printf("Size of pthread_mutex_t: %zu\n", sizeof(pthread_mutex_t));
  return 0;
}
