
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  printf("parent pid: %d\n", getpid());

  int fpid = fork();
  switch (fpid) {
  case -1:
    perror("fork");
    return EXIT_FAILURE;
  case 0:
    // child
    printf("child pid: %d\n", getpid());
    exit(77);
  default:
    // parent
    pause();
  }
  return EXIT_SUCCESS;
}
