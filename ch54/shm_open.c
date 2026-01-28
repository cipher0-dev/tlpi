#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define SIZE 100

int main(int argc, char *argv[]) {
  puts("+shm_open");
  int fd = shm_open("/foobar", O_CREAT | O_RDWR, 0600);
  puts("-shm_open");
  assert(fd >= 0 && "bad open");

  puts("+ftruncate");
  int result = ftruncate(fd, SIZE);
  puts("-ftruncate");
  assert(result == 0 && "bad truncation");

  puts("+mmap");
  char *addr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  puts("-mmap");
  assert(addr != MAP_FAILED && addr != NULL && "bad mapping");

  // write some arbitrary value to the map
  addr[0] = 5;

  return EXIT_SUCCESS;
}
