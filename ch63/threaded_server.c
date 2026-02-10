#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/stat.h>

#include <bits/pthreadtypes.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>

// creates a socket, binds it, and listens to a port
// returns the fd for the socket
int bind_and_listen(int port) {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(fd >= 0);

  int opt = 1;
  int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  assert(ret >= 0);

  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr.s_addr = htonl(INADDR_ANY),
  };
  ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  assert(ret >= 0);

  ret = listen(fd, 16);
  assert(ret >= 0);

  printf("Listening on 0.0.0.0:%d\n", port);

  return fd;
}

static int is_socket_fd(int fd) {
  struct stat st;
  if (fstat(fd, &st) == -1) {
    return 0;
  }
  return S_ISSOCK(st.st_mode);
}

static int is_listening_socket(int fd) {
  int val;
  socklen_t len = sizeof(val);

  if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1) {
    return 0;
  }

  return val != 0;
}

typedef void (*fd_iter_cb)(int fd);

void for_each_open_sock(fd_iter_cb cb) {
  DIR *dir = opendir("/proc/self/fd");
  if (dir) {
    struct dirent *de;

    while ((de = readdir(dir)) != NULL) {
      if (de->d_name[0] == '.') {
        continue;
      }

      int fd = atoi(de->d_name);

      /* Skip the directory FD used by opendir itself */
      if (fd == dirfd(dir)) {
        continue;
      }

      if (!is_socket_fd(fd)) {
        continue;
      }

      if (is_listening_socket(fd)) {
        continue;
      }

      cb(fd);
    }

    closedir(dir);
    return;
  }
}

pthread_mutex_t conn_state_mu;
int *conn_state = NULL;

int set_sock_state(int fd, int state) {
  pthread_mutex_lock(&conn_state_mu);
  conn_state[fd] = state;
  pthread_mutex_unlock(&conn_state_mu);
  return state;
}

int get_sock_state(int fd) {
  pthread_mutex_lock(&conn_state_mu);
  int state = conn_state[fd];
  pthread_mutex_unlock(&conn_state_mu);
  return state;
}

void print_sock_state(int fd) {
  printf("sock %d -> %d\n", fd, get_sock_state(fd));
}

void init_conn_state() {
  pthread_mutex_init(&conn_state_mu, NULL);
  struct rlimit r;
  getrlimit(RLIMIT_NOFILE, &r);
  conn_state = calloc(r.rlim_cur, sizeof(int));
}

#define BUF_SIZE 10 * 1 << 20

// thread that handles the connection itself
void *handle_conn(void *arg) {
  int fd = (int)(intptr_t)arg;
  printf("handleing connection: %d\n", fd);

  struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
  int ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  assert(ret >= 0);
  ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
  assert(ret >= 0);

  char *buf = malloc(BUF_SIZE);
  int n = 0;
  for (;;) {
    int state = get_sock_state(fd);
    errno = 0;
    switch (state) {
    case 1:
      sleep(1);
      break;
    case 2:
      set_sock_state(fd, 0);
      char smol_buf[256];
      strcpy(smol_buf, "Hello, world!\n");
      n = write(fd, smol_buf, 14);
      printf("write(%d) -> %d (%d: %s)\n", fd, n, errno, strerror(errno));
      break;
    default:
      n = read(fd, buf, BUF_SIZE);
      if (n == 0) {
        goto cleanup;
      }
      if (errno == EAGAIN) {
        continue;
      }

      // printf("read(%d) -> %d (%d: %s)\n", fd, n, errno, strerror(errno));
      break;
    }
  }

cleanup:
  printf("closing fd: %d\n", fd);
  close(fd);
  return NULL;
}

// thread that accepts connections and dispatches them
void *dispatch(void *arg) {
  int fd = (int)(intptr_t)arg;
  for (;;) {
    int cfd = accept(fd, NULL, NULL);
    assert(cfd > 2);

    pthread_t t;
    int ret = pthread_create(&t, NULL, &handle_conn, (void *)(intptr_t)cfd);
    assert(ret == 0);
  }
  return NULL;
}

#define IN_BUF_SIZE 80
void handle_input() {
  char line[IN_BUF_SIZE];
  char cmd[8];
  int value;

  for (;;) {
    ssize_t n = read(STDIN_FILENO, line, sizeof(line));
    assert(n >= 0);

    int ret = sscanf(line, "%7s %d", cmd, &value);
    if (ret < 1) {
      printf("bad command: %s\n", line);
      continue;
    }

    if (strncmp("block", cmd, sizeof(cmd)) == 0 ||
        strncmp("b", cmd, sizeof(cmd)) == 0) {
      set_sock_state(value, 1);
    } else if (strncmp("unblock", cmd, sizeof(cmd)) == 0 ||
               strncmp("u", cmd, sizeof(cmd)) == 0) {
      set_sock_state(value, 0);
    } else if (strncmp("write", cmd, sizeof(cmd)) == 0 ||
               strncmp("w", cmd, sizeof(cmd)) == 0) {
      set_sock_state(value, 2);
    } else if (strncmp("print", cmd, sizeof(cmd)) == 0 ||
               strncmp("p", cmd, sizeof(cmd)) == 0) {
      puts("SOCKET_STATE:");
      for_each_open_sock(print_sock_state);
    } else {
      printf("unknown command: %s\n", line);
    }
  }
}

int main(int argc, char *argv[]) {
  init_conn_state();

  assert(argc == 2);

  int port = atoi(argv[1]);
  assert(port > 0);

  int fd = bind_and_listen(port);

  pthread_t t;
  pthread_create(&t, NULL, &dispatch, (void *)(intptr_t)fd);

  handle_input();

  return EXIT_SUCCESS;
}
