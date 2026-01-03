#include <stdlib.h>
#define _GNU_SOURCE
#include <grp.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  uid_t ruid = getuid();
  uid_t euid = geteuid();
  gid_t rgid = getgid();
  gid_t egid = getegid();

  printf("UIDs:\n");
  printf("  real      = %d\n", ruid);
  printf("  effective = %d\n", euid);

  printf("GIDs:\n");
  printf("  real      = %d\n", rgid);
  printf("  effective = %d\n", egid);

  int ngroups = getgroups(0, NULL);
  if (ngroups < 0) {
    perror("getgroups");
    return EXIT_FAILURE;
  }

  gid_t groups[ngroups];
  if (getgroups(ngroups, groups) < 0) {
    perror("getgroups");
    return EXIT_FAILURE;
  }

  printf("Supplemental groups (%d):\n", ngroups);
  for (int i = 0; i < ngroups; i++) {
    printf("  [%d] %d\n", i, groups[i]);
  }

  return EXIT_SUCCESS;
}
