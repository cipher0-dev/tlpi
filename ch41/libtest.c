#include <unistd.h>

size_t __strlen(const char *s) {
  const char *p = s;
  while (*p)
    p++;
  return (size_t)(p - s);
}

int __printf(const char *out) {
  write(STDOUT_FILENO, "!! ", 3);
  return write(STDOUT_FILENO, out, __strlen(out));
};

extern int printf(const char *format, ...) { return __printf(format); };

extern void foo() { __printf("foo: called from the lib, using __printf\n"); }
extern void bar() { printf("bar: called from the lib, using printf\n"); }
