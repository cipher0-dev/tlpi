#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  int result = sem_unlink("/ch53-test");
  assert(result == 0 && "semaphore failed to unlink");

  puts("deleted semaphore");

  return EXIT_SUCCESS;
}
