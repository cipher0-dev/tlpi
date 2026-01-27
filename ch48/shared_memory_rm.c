#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  int id;
  int result;

  id = atoi(argv[1]);

  result = shmctl(id, IPC_RMID, NULL);
  printf("result: %d\n", result);
  return EXIT_SUCCESS;
}
