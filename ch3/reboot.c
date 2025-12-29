#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/reboot.h>
#include <sys/syscall.h>

int main(int argc, char *argv[]) {
  int result = syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2C,
                       LINUX_REBOOT_CMD_RESTART, NULL);
  if (result < 0) {
    perror("reboot");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
