#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

void dump_hex32(const void *p) {
  const unsigned char *b = (const unsigned char *)p;

  for (int i = 0; i < 32; i++) {
    printf("%02x ", b[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int id;
  int value;
  void *base;

  id = atoi(argv[1]);
  value = atoi(argv[2]);

  // base address - where we attached the shm to
  // base@0 - storing test value
  // base@8 - 64bit alligned pointer based off of base
  // base@24 - storing of test value

  base = (void *)shmat(id, NULL, 0);
  printf("base: %p\n", base);
  uint64_t *p = base;
  p += 8;
  printf("   p: %p\n", p);

  dump_hex32(base);

  if (*p == 0) {
    printf("p is null, not reading\n");
    *p = 24;
  } else {
    uint64_t tmp;
    tmp = *(uint64_t *)((uint64_t)base + *p);
    printf("current value at **p: %ld\n", tmp);
  }

  printf("writing to **p: %d\n", value);
  uint64_t *tmp;
  tmp = (uint64_t *)((uint64_t)base + *p);
  *tmp = value;

  dump_hex32(base);

  return EXIT_SUCCESS;
}
