
#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  sem_t *sem = sem_open("/ch53-test", O_RDWR);
  assert(sem != SEM_FAILED && "semaphore failed to open");

  int result = sem_post(sem);
  assert(result == 0 && "semaphore failed post");

  puts("released semaphore");

  return EXIT_SUCCESS;
}
