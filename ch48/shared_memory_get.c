#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main() {
  int id;
  id = shmget(IPC_PRIVATE, 1000000, 0600);
  printf("id: %d\n", id);
  return EXIT_SUCCESS;
}
