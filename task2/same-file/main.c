
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int sameFile(const char* path1, const char* path2) {
    struct stat stat1;
    struct stat stat2;
    if (stat(path1, &stat1) != 0 || stat(path2, &stat2) != 0) {
        return 0;
    }
    return (stat1.st_ino == stat2.st_ino);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 1;
    }
    const char* path1 = argv[1];
    const char* path2 = argv[2];
    if (sameFile(path1, path2)) {
        printf("yes");
    } else {
        printf("no");
    }
    return 0;
}
