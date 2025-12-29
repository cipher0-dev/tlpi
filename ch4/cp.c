#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

ssize_t write_all(int fd, const void *buf, size_t count) {
  const char *p = buf;
  size_t total = 0;

  while (total < count) {
    ssize_t n = write(fd, p + total, count - total);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }
    total += n;
  }

  return total;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: %s <src> <dest>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *src = argv[1];
  char *dest = argv[2];

  // open input file
  int src_fd = open(src, O_RDONLY | O_CLOEXEC);
  if (src_fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  // open output file
  int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (dest_fd < 0) {
    close(src_fd);
    perror("open");
    return EXIT_FAILURE;
  }

  // determine block size of output file's filesystem
  struct stat st;
  if (fstat(dest_fd, &st) == -1) {
    close(src_fd);
    close(dest_fd);
    perror("fstat");
    return EXIT_FAILURE;
  }

  size_t block_size = st.st_blksize;

  char *buf = malloc(block_size);
  if (!buf) {
    close(src_fd);
    close(dest_fd);
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  for (;;) {
    // read block from input file
    ssize_t n_read = read(src_fd, buf, block_size);
    if (n_read < 0) {
      if (errno == EINTR) {
        continue;
      }

      free(buf);
      close(src_fd);
      close(dest_fd);
      perror("read");
      return EXIT_FAILURE;
    } else if (n_read == 0) {
      // done reading
      break;
    }

    bool is_all_zeros = true;
    for (int i = 0; i < n_read; i++) {
      if (buf[i] != '\0') {
        is_all_zeros = false;
        break;
      }
    }

    if (is_all_zeros) {
      // seek to next block vs writing zeros
      if (lseek(dest_fd, n_read, SEEK_CUR) < 0) {
        free(buf);
        close(src_fd);
        close(dest_fd);
        perror("lseek");
        return EXIT_FAILURE;
      }
    } else {
      // write block to output file
      if (write_all(dest_fd, buf, n_read) < 0) {
        free(buf);
        close(src_fd);
        close(dest_fd);
        perror("write");
        return EXIT_FAILURE;
      }
    }
  }

  ftruncate(dest_fd, lseek(dest_fd, 0, SEEK_CUR));

  // cleanup
  free(buf);
  int res1 = close(src_fd);
  int res2 = close(dest_fd);
  if (res1 < 0 || res2 < 0) {
    perror("close");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
