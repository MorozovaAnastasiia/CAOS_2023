#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 256

void tail(const char* filename) {
    int fd = open(filename, O_RDONLY);
    char buf;
    int a;
    while ((a = read(fd, &buf, 1)) > 0) {
        printf("%c", buf);
        fflush(stdout);
    }
    off_t off = lseek(fd, 0, SEEK_CUR);
    close(fd);
    int newfd;
    while ((newfd = open(filename, O_RDONLY)) >= 0) {
        lseek(newfd, off, SEEK_SET);
        while ((a = read(newfd, &buf, 1)) > 0) {
            printf("%c", buf);
            fflush(stdout);
        }
        off = lseek(newfd, 0, SEEK_CUR);
        close(newfd);
        usleep(15);
    }
}

int main(int argc, char* argv[]) {
    const char* filename = argv[1];
    tail(filename);
    return 0;
}
