#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

void dump_hex32(const void *p) {
  auto b = (const unsigned char *)p;

  for (auto i = 0; i < 32; i++) {
    auto s = (i % 8 == 0 && i != 0) ? " |" : "";
    printf("%s %02x", s, b[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  auto id = atoi(argv[1]);
  auto value = atoi(argv[2]);

  uint64_t *base = shmat(id, NULL, 0);
  printf("base: %p\n", base);
  auto p = base + 1;
  printf("   p: %p\n", p);

  dump_hex32(base);

  if (*p == 0) {
    printf("p is null, not reading\n");
    // Store an offset into the shared memory base address vs the actual pointer
    // value since the memory is likely to be located in different address
    // ranges every time it is attached. This allows pointers to be position
    // independent.
    //
    // Here we use the value 3 to be 24 bytes past the base.
    *p = 3;
  } else {
    printf("current value at **p: %ld\n", *(base + *p));
  }

  printf("writing to **p: %d\n", value);
  *(base + *p) = value;

  dump_hex32(base);

  return EXIT_SUCCESS;
}
