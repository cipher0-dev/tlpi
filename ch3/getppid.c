#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t p = getppid();
  printf("getppid -> %d\n", p);
  return 0;
}
