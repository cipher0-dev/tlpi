
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(int argc, char *argv[]) {
  struct utsname buf;
  if (uname(&buf) < 0) {
    perror("uname");
    return EXIT_FAILURE;
  }
  printf("uname result:\n");
  printf("  sysname: %s\n", buf.sysname);
  printf("  nodename: %s\n", buf.nodename);
  printf("  release: %s\n", buf.release);
  printf("  version: %s\n", buf.version);
  printf("  machine: %s\n", buf.machine);
  printf("  __domainname: %s\n", buf.__domainname);
  return EXIT_SUCCESS;
}
