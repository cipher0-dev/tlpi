#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  assert(argc == 3);

  char *file_path_a = argv[1];
  char *file_path_b = argv[2];
  assert(strlen(file_path_a) > 0);
  assert(strlen(file_path_b) > 0);

  int fd_a = open(file_path_a, O_RDONLY);
  int fd_b = open(file_path_b, O_WRONLY);
  assert(fd_a >= 3);
  assert(fd_b >= 3);

  struct stat sb;
  int result = fstat(fd_a, &sb);
  assert(result == 0);

  ssize_t n = sendfile(fd_b, fd_a, 0, sb.st_size);
  assert(n >= 0);

  return EXIT_SUCCESS;
}
