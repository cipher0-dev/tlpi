#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

struct linux_dirent64 {
  ino64_t d_ino;           /* 64-bit inode number */
  off64_t d_off;           /* Not an offset; see getdents() */
  unsigned short d_reclen; /* Size of this dirent */
  unsigned char d_type;    /* File type */
  char d_name[];           /* Filename (null-terminated) */
};

bool is_numeric(char *str) {
  int cur = 0;
  while (str[cur] != '\0') {
    if (str[cur] > '9' || str[cur] < '0') {
      return false;
    }
    cur++;
  }
  return true;
}

char *create_indent(int n) {
  int count = n * 2;

  // 1. Allocate space for (2 * n) spaces + 1 null terminator
  char *str = malloc(count + 1);
  if (!str)
    return NULL;

  // 2. Fill the entire block with spaces
  memset(str, ' ', count);

  // 3. Manually add the null terminator at the end
  str[count] = '\0';

  return str;
}

int get_ppid(char *status_buf, size_t status_size, char *ppid_buf,
             size_t ppid_size) {
  enum states {
    Unknown,
    EOL,
    P,
    PP,
    PPi,
    PPid,
    PPidC,
    PPidCT,
  };

  enum states state = Unknown;
  int j;

  for (int i = 0; i < status_size; i++) {
    switch (state) {
    case Unknown:
      if (status_buf[i] == '\n') {
        state = EOL;
      }
      break;
    case EOL:
      if (status_buf[i] == 'P') {
        state = P;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case P:
      if (status_buf[i] == 'P') {
        state = PP;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case PP:
      if (status_buf[i] == 'i') {
        state = PPi;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case PPi:
      if (status_buf[i] == 'd') {
        state = PPid;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case PPid:
      if (status_buf[i] == ':') {
        state = PPidC;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case PPidC:
      if (status_buf[i] == '\t') {
        state = PPidCT;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case PPidCT:
      for (j = i; j < status_size; j++) {
        if (status_buf[j] == '\n') {
          break;
        }
      }
      memcpy(ppid_buf, status_buf + i, j - i);
      ppid_buf[j - i] = '\0';
      return j - i;
    }
  }
  return -1;
}

int get_name(char *status_buf, size_t status_size, char *name_buf,
             size_t name_size) {
  enum states {
    Unknown,
    EOL,
    N,
    Na,
    Nam,
    Name,
    NameC,
    NameCT,
  };

  enum states state = EOL;
  int j;

  for (int i = 0; i < status_size; i++) {
    switch (state) {
    case Unknown:
      if (status_buf[i] == '\n') {
        state = EOL;
      }
      break;
    case EOL:
      if (status_buf[i] == 'N') {
        state = N;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case N:
      if (status_buf[i] == 'a') {
        state = Na;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case Na:
      if (status_buf[i] == 'm') {
        state = Nam;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case Nam:
      if (status_buf[i] == 'e') {
        state = Name;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case Name:
      if (status_buf[i] == ':') {
        state = NameC;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case NameC:
      if (status_buf[i] == '\t') {
        state = NameCT;
      } else if (status_buf[i] == '\n') {
        state = EOL;
      } else {
        state = Unknown;
      }
      break;
    case NameCT:
      for (j = i; j < status_size; j++) {
        if (status_buf[j] == '\n') {
          break;
        }
      }
      memcpy(name_buf, status_buf + i, j - i);
      name_buf[j - i] = '\0';
      return j - i;
    }
  }
  return -1;
}

typedef struct {
  int key;
  char *name;
  int *children;
} pid_hash;

void print_tree(pid_hash *hash, int pid, int indent) {
  pid_hash elem = hmgets(hash, pid);
  printf("%s%s (%d)\n", create_indent(indent), elem.name, elem.key);

  for (int i = 0; i < arrlen(elem.children); i++) {
    print_tree(hash, elem.children[i], indent + 1);
  }
}

int main(int argc, char *argv[]) {
  int fd = open("/proc", O_RDONLY | O_DIRECTORY);
  if (fd < 0) {
    perror("open(/proc)");
    return EXIT_FAILURE;
  }

  char dirent_buf[1 << 15];
  int nread;

  pid_hash *hash = NULL;
  int count = 0;

  while ((nread = getdents64(fd, dirent_buf, sizeof(dirent_buf))) > 0) {
    int bpos = 0;

    while (bpos < nread) {
      struct linux_dirent64 *d = (struct linux_dirent64 *)(dirent_buf + bpos);
      bpos += d->d_reclen;

      if (!is_numeric(d->d_name)) {
        continue;
      }
      count++;
      int pid = atoi(d->d_name);
      if (pid < 1) {
        printf("bad pid atoi\n");
        return EXIT_FAILURE;
      }

      char status_path[1 << 10];
      snprintf(status_path, sizeof(status_path), "/proc/%s/status", d->d_name);

      int status_fd = open(status_path, O_RDONLY);
      if (status_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
      }

      // NOTE: assume the size of status file for simplicity
      char status_buf[1 << 12];
      if (read(status_fd, status_buf, sizeof(status_buf)) < 0) {
        perror("read");
        return EXIT_FAILURE;
      }

      // NOTE: assume the size of name simplicity
      char ppid_buf[10];
      if (get_ppid(status_buf, sizeof(status_buf), ppid_buf, sizeof(ppid_buf)) <
          1) {
        printf("bad ppid\n");
        return EXIT_FAILURE;
      }
      int ppid = atoi(ppid_buf);
      if (ppid < 0) {
        printf("bad ppid atoi: %s, pid: %d\n", ppid_buf, pid);
        return EXIT_FAILURE;
      }

      // NOTE: assume the size of name simplicity
      char name_buf[1 << 10];
      if (get_name(status_buf, sizeof(status_buf), name_buf, sizeof(name_buf)) <
          0) {
        printf("bad name\n");
        return EXIT_FAILURE;
      }

      // printf("handling pid: %d, name: %s, ppid: %d\n", pid, name_buf, ppid);

      pid_hash pid_elem = hmgets(hash, pid);
      pid_elem.key = pid;
      pid_elem.name = strdup(name_buf);
      // printf("  pid_elem: key: %d, name: %s, children len: %td\n",
      // pid_elem.key,
      //        pid_elem.name, arrlen(pid_elem.children));
      hmputs(hash, pid_elem);

      pid_hash ppid_elem = hmgets(hash, ppid);
      ppid_elem.key = ppid;
      arrput(ppid_elem.children, pid);
      // printf("  ppid_elem: key: %d, name: %s, children len: %td\n",
      //        ppid_elem.key, ppid_elem.name, arrlen(ppid_elem.children));
      hmputs(hash, ppid_elem);
    }
  }

  // print out the datastructure as a tree
  print_tree(hash, 0, 0);

  return EXIT_SUCCESS;
}
