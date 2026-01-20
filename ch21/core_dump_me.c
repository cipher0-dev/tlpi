// This program loops forever, expecting to be core dumped with gcore.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("core dump me!!!\n");
  printf("  gcore %d\n", getpid());

  while (1) {
    sleep(1);
    printf(".");
    fflush(stdout);
  }

  return EXIT_SUCCESS;
}
