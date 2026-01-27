#define _GNU_SOURCE
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fsuid.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  {
    uid_t ruid = 0;
    uid_t euid = 0;
    uid_t suid = 0;
    if (getresuid(&ruid, &euid, &suid) < 0) {
      perror("getresuid");
      return EXIT_FAILURE;
    }
    uid_t fuid = setfsuid(-1);

    printf("UIDs:\n");
    printf("  real      = %d\n", ruid);
    printf("  effective = %d\n", euid);
    printf("  saved = %d\n", suid);
    printf("  fs = %d\n", fuid);
  }

  {
    uid_t rgid = 0;
    uid_t egid = 0;
    uid_t sgid = 0;
    if (getresgid(&rgid, &egid, &sgid) < 0) {
      perror("getresgid");
      return EXIT_FAILURE;
    }
    uid_t fgid = setfsgid(-1);

    printf("GIDs:\n");
    printf("  real      = %d\n", rgid);
    printf("  effective = %d\n", egid);
    printf("  saved = %d\n", sgid);
    printf("  fs = %d\n", fgid);
  }

  {
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
  }

  return EXIT_SUCCESS;
}
