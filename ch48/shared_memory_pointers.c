#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

void dump_hex32(const void *p) {
  const unsigned char *b = (const unsigned char *)p;

  for (int i = 0; i < 32; i++) {
    char *s = (i % 8 == 0 && i != 0) ? " |" : "";
    printf("%s %02x", s, b[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int id = atoi(argv[1]);
  int value = atoi(argv[2]);

  uint64_t *base = shmat(id, NULL, 0);
  printf("base: %p\n", base);
  uint64_t *p = base + 1;
  printf("   p: %p\n", p);

  dump_hex32(base);

  if (*p == 0) {
    printf("p is null, not reading\n");
    // Store an array offset into the shared memory vs the actual pointer value
    // since the memory is likely to be located in different address ranges
    // every time it is mapped.
    //
    // Here we use the value 3 to be 24 bytes passed the base.
    *p = 3;
  } else {
    printf("current value at **p: %ld\n", *(base + *p));
  }

  printf("writing to **p: %d\n", value);
  *(base + *p) = value;

  dump_hex32(base);

  return EXIT_SUCCESS;
}
