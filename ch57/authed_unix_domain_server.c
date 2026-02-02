#define _GNU_SOURCE
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/uds_echo.sock"
#define BACKLOG 16
#define BUF_SIZE 4096

static void *handle_connection(void *arg) {
  int fd = (int)(intptr_t)arg;

  struct ucred cred;
  socklen_t len = sizeof(cred);

  if (getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) == -1) {
    perror("getsockopt(SO_PEERCRED)");
    close(fd);
    return NULL;
  }

  printf("client connected: pid=%d uid=%d gid=%d\n", cred.pid, cred.uid,
         cred.gid);

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    close(fd);
    return NULL;
  }

  for (;;) {
    /* socket -> pipe */
    ssize_t n = splice(fd, NULL, pipefd[1], NULL, 64 * 1024,
                       SPLICE_F_MOVE | SPLICE_F_MORE);
    if (n == 0) {
      /* client closed */
      break;
    }
    if (n < 0) {
      perror("splice socket->pipe");
      break;
    }

    ssize_t remaining = n;
    while (remaining > 0) {
      /* pipe -> socket */
      ssize_t m = splice(pipefd[0], NULL, fd, NULL, remaining,
                         SPLICE_F_MOVE | SPLICE_F_MORE);
      if (m < 0) {
        perror("splice pipe->socket");
        goto out;
      }
      remaining -= m;
    }
  }

out:
  printf("client disconnected: pid=%d\n", cred.pid);
  close(pipefd[0]);
  close(pipefd[1]);
  close(fd);
  return NULL;
}

int main(void) {
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  unlink(SOCK_PATH);

  struct sockaddr_un addr = {
      .sun_family = AF_UNIX,
  };
  strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (listen(fd, BACKLOG) == -1) {
    perror("listen");
    close(fd);
    exit(EXIT_FAILURE);
  }

  printf("listening on %s\n", SOCK_PATH);

  for (;;) {
    int conn_fd = accept(fd, NULL, NULL);
    if (conn_fd == -1) {
      perror("accept");
      continue;
    }

    pthread_t tid;
    if (pthread_create(&tid, NULL, handle_connection,
                       (void *)(intptr_t)conn_fd) != 0) {
      perror("pthread_create");
      close(conn_fd);
      continue;
    }

    pthread_detach(tid);
  }

  close(fd);
  unlink(SOCK_PATH);
  return EXIT_SUCCESS;
}
