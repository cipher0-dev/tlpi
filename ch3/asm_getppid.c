// Minimal stack-fail handler
void __stack_chk_fail(void) {
  // Exit immediately (syscall 60)
  asm volatile("mov $60, %rax\n\t"  // SYS_exit
               "xor %rdi, %rdi\n\t" // exit status 0
               "syscall\n\t");

  // Should never reach here
  while (1) {
  }
}

// Convert a number to a string (decimal)
static void utoa(long n, char *buf) {
  char tmp[20];
  int i = 0;
  if (n == 0) {
    buf[0] = '0';
    buf[1] = '\n';
    buf[2] = '\0';
    return;
  }
  while (n > 0) {
    tmp[i++] = '0' + (n % 10);
    n /= 10;
  }
  int j = 0;
  while (i--)
    buf[j++] = tmp[i];
  buf[j++] = '\n';
  buf[j] = '\0';
}

void _start(void) {
  long p;

  // getppid syscall
  asm volatile("mov $110, %%rax\n\t"
               "syscall\n\t"
               "mov %%rax, %0\n\t"
               : "=r"(p)
               :
               : "rax");

  char buf[47] = "asm_getppid -> ";
  utoa(p, &buf[15]);

  // write syscall: write(1, buf, len)
  long len = 0;
  while (buf[len])
    len++;

  asm volatile("mov $1, %%rax\n\t" // SYS_write
               "mov $1, %%rdi\n\t" // stdout
               "mov %0, %%rsi\n\t" // buffer
               "mov %1, %%rdx\n\t" // length
               "syscall\n\t"
               :
               : "r"(buf), "r"(len)
               : "rax", "rdi", "rsi", "rdx");

  // exit syscall
  asm volatile("mov $60, %%rax\n\t"   // SYS_exit
               "xor %%rdi, %%rdi\n\t" // status 0
               "syscall\n\t"
               :
               :
               : "rax", "rdi");
}
