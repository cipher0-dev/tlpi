#include <errno.h>
#include <fcntl.h>
#include <linux/kcmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

char *mode_name(mode_t mode) {
  if (S_ISREG(mode))
    return "regular file";
  else if (S_ISDIR(mode))
    return "directory\n";
  else if (S_ISLNK(mode))
    return "symlink\n";
  else if (S_ISCHR(mode))
    return "character device\n";
  else if (S_ISBLK(mode))
    return "block device\n";
  else if (S_ISFIFO(mode))
    return "FIFO / pipe\n";
  else if (S_ISSOCK(mode))
    return "socket\n";
  else
    return "unknown type\n";
}

void print_file_description_flags(int flags) {
  switch (flags & O_ACCMODE) {
  case O_RDONLY:
    printf("    O_RDONLY\n");
    break;
  case O_WRONLY:
    printf("    O_WRONLY\n");
    break;
  case O_RDWR:
    printf("    O_RDWR\n");
    break;
  default:
    printf("    UNKNOWN ACCESS MODE\n");
    break;
  }

  // Other flags (bitmask)
  if (flags & O_APPEND)
    printf("    O_APPEND\n");
  if (flags & O_NONBLOCK)
    printf("    O_NONBLOCK\n");
#ifdef O_DSYNC
  if (flags & O_DSYNC)
    printf("    O_DSYNC\n");
#endif
#ifdef O_SYNC
  if (flags & O_SYNC)
    printf("    O_SYNC\n");
#endif
#ifdef O_RSYNC
  if (flags & O_RSYNC)
    printf("    O_RSYNC\n");
#endif
  if (flags & O_CREAT)
    printf("    O_CREAT\n");
  if (flags & O_TRUNC)
    printf("    O_TRUNC\n");
  if (flags & O_EXCL)
    printf("    O_EXCL\n");
#ifdef O_CLOEXEC
  if (flags & O_CLOEXEC)
    printf("    O_CLOEXEC\n");
#endif
}

void print_file_description(int fd) {
  // description:
  //   - offset
  //   - status flags
  //   - inode ptr
  off_t off = lseek(fd, 0, SEEK_CUR);
  int flags = fcntl(fd, F_GETFL);
  printf("  offset: %ld\n", off);
  printf("  flags: %x\n", flags);
  print_file_description_flags(flags);

  struct stat st;
  fstat(fd, &st);
  printf("  inode num: %ld\n", st.st_ino);
  printf("  device: %ld\n", st.st_dev);
  printf("  mode: %s\n", mode_name(st.st_mode));
}

int same_file_description(int fd1, int fd2) {
  long ret = syscall(SYS_kcmp, getpid(), getpid(), KCMP_FILE, fd1, fd2);

  if (ret == -1) {
    perror("kcmp");
    return -1;
  }

  return ret == 0;
}

void compare_fds(int fd1, int fd2) {
  // descriptor:
  //   - fd flags
  //   - description ptr
  printf("fd1: %d\n", fd1);
  int fd_flags_1 = fcntl(fd1, F_GETFD);
  printf("  descriptor flags: %x\n", fd_flags_1);

  printf("fd2: %d\n", fd2);
  int fd_flags_2 = fcntl(fd2, F_GETFD);
  printf("  descriptor flags: %x\n\n", fd_flags_2);

  if (same_file_description(fd1, fd2) == true) {
    printf("description is shared\n\n");
  } else {
    printf("description is not shared\n\n");
  }

  printf("description(fd1):\n");
  print_file_description(fd1);
  printf("description(fd2):\n");
  print_file_description(fd2);
}

int is_fd_open(int fd) { return fcntl(fd, F_GETFD) != -1; }

int my_dup(int oldfd) {
  if (fcntl(oldfd, F_GETFL) < 0) {
    return EBADF;
  }
  return fcntl(oldfd, F_DUPFD);
}

int my_dup2(int oldfd, int newfd) {
  if (oldfd == newfd) {
    if (fcntl(oldfd, F_GETFL) < 0) {
      errno = EBADF;
      return -1;
    }
    return oldfd;
  }
  // NOTE: closing the newfd and recreating it with fcntl is not atomic and so
  // the actual newfd might be greater than what is provided if a race occured
  if (is_fd_open(newfd)) {
    if (close(newfd) < 0) {
      perror("close");
      return -1;
    };
  }
  return fcntl(oldfd, F_DUPFD, newfd);
}

void print_usage(const char *prog) {
  fprintf(stderr, "usage: %s <file>\n", prog);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  printf("# dup2 SYSCALL\n\n");

  {
    int fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
    if (fd1 < 0) {
      perror("open");
      return EXIT_FAILURE;
    }

    int fd2 = dup2(fd1, 123);
    if (fd2 < 0) {
      perror("dup2");
      return EXIT_FAILURE;
    }

    compare_fds(fd1, fd2);
  }

  printf("\n# my_dup2 CALL\n\n");

  {
    int fd1 = open(argv[1], O_RDWR | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
    if (fd1 < 0) {
      perror("open");
      return EXIT_FAILURE;
    }

    int fd2 = my_dup2(fd1, 123);
    if (fd2 < 0) {
      perror("my_dup2");
      return EXIT_FAILURE;
    }

    compare_fds(fd1, fd2);
  }

  return EXIT_SUCCESS;
}
