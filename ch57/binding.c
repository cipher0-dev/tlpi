#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char *argv[]) {
  assert(argc == 2);
  char *path = argv[1];

  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  assert(fd != -1 || (perror("socket"), 0));

  struct sockaddr_un addr = {
      .sun_family = AF_UNIX,
  };
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

  int result = bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
  assert(result != -1 || (perror("bind"), 0));

  return EXIT_SUCCESS;
}
