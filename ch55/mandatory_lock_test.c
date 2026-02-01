#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static void lock_entire_file(int fd) {
  struct flock fl = {
      .l_type = F_WRLCK,
      .l_whence = SEEK_SET,
      .l_start = 0,
      .l_len = 0, // 0 == to EOF
  };

  if (fcntl(fd, F_SETLK, &fl) == -1) {
    perror("fcntl(F_SETLK)");
    exit(EXIT_FAILURE);
  }
}

static void test_case(const char *label, mode_t mode) {
  printf("\n=== %s ===\n", label);

  char template[] = "./build/mandlock-testXXXXXX";
  int fd = mkstemp(template);
  if (fd == -1) {
    perror("mkstemp");
    exit(EXIT_FAILURE);
  }

  unlink(template); // auto-cleanup

  if (fchmod(fd, mode) == -1) {
    perror("fchmod");
    exit(EXIT_FAILURE);
  }

  write(fd, "hello\n", 6);
  lseek(fd, 0, SEEK_SET);

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // child: take exclusive lock and sleep
    printf("child: taking exclusive lock\n");
    lock_entire_file(fd);
    printf("child: lock acquired, sleeping...\n");
    sleep(10);
    printf("child: exiting\n");
    _exit(0);
  }

  sleep(1); // ensure child has lock

  printf("parent: attempting read (this may block)...\n");

  char buf[16];
  ssize_t n = read(fd, buf, sizeof(buf));

  if (n == -1) {
    perror("read");
  } else {
    printf("parent: read returned %zd bytes: %.*s\n", n, (int)n, buf);
  }

  waitpid(pid, NULL, 0);
  close(fd);
}

int main(void) {
  // Regular permissions -> advisory locking
  test_case("advisory locking (should NOT block)", 0644);

  // Mandatory locking:
  //  - setgid bit ON
  //  - group execute bit OFF
  test_case("mandatory locking (read SHOULD block)",
            02644 /* setgid + rw-r--r-- */);

  return EXIT_SUCCESS;
}
