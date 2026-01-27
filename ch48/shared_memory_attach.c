#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  int id;
  char *addr;
  void *result;

  id = atoi(argv[1]);

  result = shmat(id, NULL, SHM_RDONLY);
  if (result == (void *)-1) {
    perror("shmat");
    return EXIT_FAILURE;
  }
  addr = (char *)result;
  printf("pointer: %p\n", addr);
  printf("first byte: %d\n", addr[0]);
  return EXIT_SUCCESS;
}
