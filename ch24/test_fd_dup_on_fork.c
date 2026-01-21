// #include <bits/time.h>
// #include <time.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int has_cloexec(int fd) {
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {
    perror("fcntl(F_GETFD)");
    return -1;
  }
  return (flags & FD_CLOEXEC) != 0;
}

int clear_cloexec(int fd) {
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1)
    return -1;

  flags &= ~FD_CLOEXEC;
  return fcntl(fd, F_SETFD, flags);
}

int main() {
  int fd = open("/tmp/foo", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  off_t offset = lseek(fd, 1000, SEEK_SET);
  if (offset < 0) {
    perror("lseek");
    return EXIT_FAILURE;
  }
  off_t cur_off;
  pid_t child_pid;

  switch (child_pid = fork()) {
  case -1:
    perror("fork");
    return EXIT_FAILURE;
  case 0:
    // child
    sleep(1);
    cur_off = lseek(fd, 2000, SEEK_CUR);
    if (cur_off == (off_t)-1) {
      perror("lseek");
    }
    printf("child, fd: %d, cloexec: %b, cur_off: %ld\n", fd, has_cloexec(fd),
           cur_off);
    break;
  default:
    cur_off = lseek(fd, 3000, SEEK_CUR);
    if (cur_off == (off_t)-1) {
      perror("lseek");
    }
    // parent
    printf("parent, fd: %d, cloexec: %b, cur_off: %ld\n", fd, has_cloexec(fd),
           cur_off);
    clear_cloexec(fd);
    waitpid(child_pid, NULL, 0);
    break;
  }

  return EXIT_SUCCESS;
}
