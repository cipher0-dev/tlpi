#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void search_proc_fds(const char *target_path) {
  DIR *proc_dir, *fd_dir;
  struct dirent *proc_entry, *fd_entry;
  char fd_path[PATH_MAX];
  char link_target[PATH_MAX];
  ssize_t len;

  proc_dir = opendir("/proc");
  if (proc_dir == NULL) {
    perror("Error opening /proc");
    return;
  }

  printf("Processes accessing: %s\n", target_path);
  printf("%-10s %-10s\n", "PID", "FD");
  printf("---------------------\n");

  // Loop through /proc
  while ((proc_entry = readdir(proc_dir)) != NULL) {
    // Check if directory name is numeric (a PID)
    if (!isdigit(proc_entry->d_name[0]))
      continue;

    // Construct path to /proc/[PID]/fd
    snprintf(fd_path, sizeof(fd_path), "/proc/%s/fd", proc_entry->d_name);

    fd_dir = opendir(fd_path);
    if (fd_dir == NULL)
      continue; // Skip if we can't open (e.g., permission denied)

    // Loop through /proc/[PID]/fd/*
    while ((fd_entry = readdir(fd_dir)) != NULL) {
      if (fd_entry->d_name[0] == '.')
        continue; // Skip . and ..

      char fd_link[PATH_MAX];
      snprintf(fd_link, sizeof(fd_link), "%s/%s", fd_path, fd_entry->d_name);

      // Read the symbolic link
      len = readlink(fd_link, link_target, sizeof(link_target) - 1);
      if (len != -1) {
        link_target[len] = '\0'; // Null-terminate the string

        // Compare link target with our target path
        if (strcmp(link_target, target_path) == 0) {
          printf("%-10s %-10s\n", proc_entry->d_name, fd_entry->d_name);
        }
      }
    }
    closedir(fd_dir);
  }
  closedir(proc_dir);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <absolute-pathname>\n", argv[0]);
    return EXIT_FAILURE;
  }

  search_proc_fds(argv[1]);

  return EXIT_SUCCESS;
}
