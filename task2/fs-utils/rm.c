#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include <assert.h>

int f(char str[], bool has_r) {
  struct stat stat_elem;
  lstat(str, &stat_elem);
  if (S_ISREG(stat_elem.st_mode) || S_ISLNK(stat_elem.st_mode)) {
    return unlink(str);
  }
  else if (S_ISDIR(stat_elem.st_mode)) {
    struct dirent* dirent_elem;
    DIR* dir = opendir(str);
    if (!has_r) {
      exit(-1);
    }
    // .
    //h
    readdir(dir);
    // ..
    readdir(dir);
    while ((dirent_elem = readdir(dir)) != NULL) {
      int s1 = strlen(str);
      int s2 = strlen(dirent_elem->d_name);
      char ans[s1 + s2 + 1];
      ans[s1] = '/';
      ans[s1 + s2 + 1] = '\0';
      for (int i = 0; i < s1; ++i) {
        ans[i] = str[i];
      }
      for (int i = 0; i < s2; ++i) {
        ans[s1 + i + 1] = dirent_elem->d_name[i];
      }
      f(ans, has_r);
    };
    closedir(dir);
    return rmdir(str);
  }
  return -1;
}

int main(int argc, char* argv[]) {
  bool has_r = false;
  for (int i = 0; i < argc - 1; ++i) {
    if (argv[i + 1][1] == 'r') {
       has_r = true;
       break;
    }
  }
  for (int i = 0; i < argc - 1; ++i) {
    if (argv[i + 1][0] == '-') continue;
    f(argv[i + 1], has_r);
  }
  return 0;
}

