#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdatomic.h>

#include <arpa/inet.h>
#include <sys/socket.h>

_Atomic uint64_t byte_count = 0;

void *reader(void *arg) {
  int fd = (int)(intptr_t)arg;
  printf("reader fd: %d\n", fd);
  char buf[256];
  for (;;) {
    errno = 0;
    int n = read(fd, &buf, sizeof(buf));
    if (n == 0) {
      return NULL;
    }
    if (errno == EAGAIN) {
      continue;
    }

    printf("read(%d) -> %d (%d: %s)\n", fd, n, errno, strerror(errno));
    break;
  }
  // NOTE: the reader doesn't close the fd on EOF in case shutdown was used
  return NULL;
}

#define BUF_SIZE 10 * 1 << 20

void *writer(void *arg) {
  int fd = (int)(intptr_t)arg;
  printf("writer fd: %d\n", fd);
  char *buf = malloc(BUF_SIZE);
  for (size_t i = 0; i < sizeof(buf); ++i) {
    buf[i] = rand() % 256;
  }

  for (;;) {
    errno = 0;
    int n = write(fd, buf, BUF_SIZE);
    if (n == 0) {
      goto cleanup;
    }
    if (n < 0) {
      printf("write(%d) -> %d (%d: %s)\n", fd, n, errno, strerror(errno));
      goto cleanup;
    }
    assert(n > 0);
    atomic_fetch_add(&byte_count, n);
  }
cleanup:
  printf("closing fd: %d\n", fd);
  close(fd);
  free(buf);
  return NULL;
}

#define NUM_THREADS 12

int main(int argc, char *argv[]) {
  assert(argc == 2);
  int port = atoi(argv[1]);
  assert(port > 0);

  for (int i = 0; i < NUM_THREADS; ++i) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(fd >= 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
    };
    int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    assert(ret == 0);

    pthread_t r, w;
    ret = pthread_create(&r, NULL, &reader, (void *)(intptr_t)fd);
    assert(ret == 0);
    ret = pthread_create(&w, NULL, &writer, (void *)(intptr_t)fd);
    assert(ret == 0);
  }

  for (;;) {
    uint64_t v = atomic_exchange(&byte_count, 0);
    printf("%lu\tbytes/s\n", v);
    sleep(1);
  }
}
