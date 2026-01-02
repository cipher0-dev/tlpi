#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PREV_INUSE 0x1
#define IS_MMAPPED 0x2
#define NON_MAIN_ARENA 0x4
#define SIZE_MASK (~(size_t)0x7)

int main(void) {
  size_t req = 0xFEED;

  void *p = malloc(req);
  if (!p) {
    perror("malloc");
    return 1;
  }

  printf("malloc(%zu) returned %p\n\n", req, p);

  /*
   * glibc layout:
   *
   * chunk:
   *   [ prev_size ]   <-- p - 16 (x86_64)
   *   [ size      ]   <-- p - 8
   *   [ user data ]   <-- p
   */

  size_t *size_field = (size_t *)p - 1;
  size_t *prev_size_field = (size_t *)p - 2;

  size_t raw_size = *size_field;
  size_t chunk_size = raw_size & SIZE_MASK;

  printf("Chunk header:\n");
  printf("  prev_size: 0x%zx (%zu)\n", *prev_size_field, *prev_size_field);

  printf("  size (raw): 0x%zx\n", raw_size);
  printf("  size (masked): %zu bytes\n", chunk_size);

  printf("\nFlags:\n");
  printf("  PREV_INUSE:     %s\n", (raw_size & PREV_INUSE) ? "yes" : "no");
  printf("  IS_MMAPPED:     %s\n", (raw_size & IS_MMAPPED) ? "yes" : "no");
  printf("  NON_MAIN_ARENA: %s\n", (raw_size & NON_MAIN_ARENA) ? "yes" : "no");

  free(p);
  return 0;
}
