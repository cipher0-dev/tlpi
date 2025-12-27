#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]) {
  pid_t p = syscall(SYS_getppid);
  printf("sys_getppid -> %d\n", p);
  return 0;
}
