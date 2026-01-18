#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("err: %p\n", SIG_ERR);
  printf("dfl: %p\n", SIG_DFL);
  printf("ign: %p\n", SIG_IGN);
  return EXIT_SUCCESS;
}
