#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

int64_t curmode = (1 << 10) - 1;
bool has_p = false;

void getAns(char currentline[]) {
  char num_to_convert[strlen(currentline)];
  for (int i = 0; i < strlen(currentline); ++i) {
    num_to_convert[i] = currentline[i];
    if (has_p) {
      if (currentline[i] == '/') {
        num_to_convert[i] = '\0';
        mkdir(num_to_convert, curmode);
        num_to_convert[i] = '/';
      }
    }
  }
  num_to_convert[strlen(currentline)] = '\0';
  if (mkdir(num_to_convert, curmode) != 0) {
    exit(5);
  }
}
void f(char currentline[]) {
  if (currentline[0] == '-' && currentline[1] == 'p') {
    has_p = true;
    return;
  }
}

int main(int argc, char* argv[]) {
  int i = 1;
  if (argv[i][0] == '-') {
    f(argv[i++]);
  }
  if ((i < argc) && (argv[i][0] == '-')) { f(argv[i++]); }
  while (true) {
    if (i >= argc) break;
    getAns(argv[i]);
    ++i;
  }
  return 0;
}

