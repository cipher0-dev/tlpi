#include <stdio.h>
#include <unistd.h>

int main() {
  long buf = fpathconf(0, _PC_PIPE_BUF);
  if (buf == -1) {
    perror("fpathconf");
    return 1;
  }
  printf("PIPE_BUF = %ld\n", buf);
  return 0;
}
