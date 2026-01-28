#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  puts("+sem_open");
  sem_t *sem = sem_open("/ch53-test", O_CREAT | O_RDWR, 0600, 4);
  puts("-sem_open");
  assert(sem != SEM_FAILED && "semaphore failed to open");

  puts("sem created");

  puts("+sem_close");
  int result = sem_close(sem);
  puts("-sem_close");
  assert(result == 0 && "close failed");

  return EXIT_SUCCESS;
}
