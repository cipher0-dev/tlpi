#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog) {
  fprintf(stderr, "usage: %s <file>\n", prog);
}

static void noop(int signo) { (void)signo; }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  (void)printf("fd: %d\n\n", fd);

  struct sigaction sa = {0};
  sa.sa_handler = noop;
  sigaction(SIGUSR1, &sa, NULL);

  (void)printf("pausing...\n\n");
  (void)pause();

  (void)printf("unlinking file: %s\n", argv[1]);
  (void)unlink(argv[1]);
  (void)printf("pausing...\n\n");
  (void)pause();

  return EXIT_SUCCESS;
}
