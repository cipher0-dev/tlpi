#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  pid_t p = syscall(SYS_getppid);
  printf("sys_getppid -> %d\n", p);
  return 0;
}
