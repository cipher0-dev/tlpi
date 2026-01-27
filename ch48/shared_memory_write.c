#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  int id;
  int value;
  char *result;

  id = atoi(argv[1]);
  value = atoi(argv[2]);

  result = (char *)shmat(id, NULL, 0);
  printf("pointer: %p\n", result);
  result[0] = value;
  return EXIT_SUCCESS;
}
