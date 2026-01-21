#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct timespec res;
  clockid_t clockid;

  int ret = clock_getres(CLOCK_REALTIME, &res);
  if (ret < 0) {
    perror("clock_getres");
    return EXIT_FAILURE;
  }
  printf("%ld %ld\n", res.tv_sec, res.tv_nsec);

  return EXIT_SUCCESS;
}
