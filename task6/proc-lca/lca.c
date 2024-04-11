#include "lca.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h> 
#include <unistd.h>

pid_t GetParentPid(char* path) {
    int current_pid = 0;
    char BUFFER[512 + 1];
    int fd = open(path, O_RDONLY);
    int current_amount = 0;
    do {
        BUFFER[current_amount] = '\0';
        char* to_grab = strstr(BUFFER, "PPid:");
        if (to_grab) {
            sscanf(to_grab, "PPid:%d", &current_pid);
            break;
        }
    } while ((current_amount = read(fd, BUFFER, 512)) > 0);
    close(fd);
    return current_pid;
}
void GetVectorOfParents(pid_t proc, int parents[MAX_TREE_DEPTH], int* current_depth) {
    pid_t current_pid;
    current_pid = proc;
    *current_depth = 0;
    do {
        char path[20];
        parents[*current_depth] = current_pid;
        size_t ans = 0;
        int n = current_pid;
        while (n != 0) {
            n /= 10;
            ++ans;
        }
        snprintf(path, 15 + ans, "/proc/%d/status", current_pid);
        ++*current_depth;
        parents[*current_depth] = 1;
        current_pid = GetParentPid(path);
    } while (current_pid != 1);
    ++*current_depth;
}
pid_t find_lca(pid_t x, pid_t y) {
    pid_t x_parents[MAX_TREE_DEPTH];
    pid_t y_parents[MAX_TREE_DEPTH];
    int current_depth_x = 0;
    GetVectorOfParents(x, x_parents, &current_depth_x);
    int current_depth_y = 0;
    GetVectorOfParents(y, y_parents, &current_depth_y);
    int i = 0;
    while (true) {
        // ok
        auto try_for_one = current_depth_x - i - 1;
        auto try_for_two = current_depth_y - i - 1;
        if (x_parents[try_for_one] != y_parents[try_for_two]) {
            return x_parents[current_depth_x - i]; 
        }
        if (try_for_one == 0) {
            return x_parents[0];
        }
        if (try_for_two == 0) {
            return y_parents[0];
        }
        ++i;
    } 
    return 1;
}
