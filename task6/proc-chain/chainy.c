#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

enum {
    MAX_ARGS_COUNT = 256,
    MAX_CHAIN_LINKS_COUNT = 256
};
const int MATRIX_SIZE = 2;
typedef struct {
    char* command;
    uint64_t argc;
    char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
    uint64_t chain_links_count;
    chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

bool checkValue(char* ch) {
    return ch == ' ' || ch == '\0';
}

typedef struct {
    int i;
    int j;
} idxes;

void create_chain(char* command, chain_t* chain) {
    int cmd = 0;
    idxes idxes1;
    idxes1.j = 0;
    idxes1.i = -1;
    char c;
    if (idxes1.j > 0 && command[idxes1.j - 1] == '\0') {
        chain->chain_links_count = cmd;
        return;
    }
    while (idxes1.j == 0 || command[idxes1.j - 1] != '\0') {
        if (idxes1.i == -1 && command[idxes1.j] != ' ') {
            idxes1.i = idxes1.j;
        }
        if (checkValue(command[idxes1.j])) {
            if (command[idxes1.i] != '|') {
                ++chain->chain_links[cmd].argc;
                char* cur_arg = calloc(idxes1.j - idxes1.i + 1, sizeof(c));
                cur_arg[idxes1.j - idxes1.i] = '\0';
                memcpy(cur_arg, command + idxes1.i, idxes1.j - idxes1.i);
                chain->chain_links[cmd].argv[chain->chain_links[cmd].argc - 1] = cur_arg;
            }
            if (command[idxes1.i] == '|' || command[idxes1.j] == '\0') {
                chain->chain_links[cmd].command = chain->chain_links[cmd].argv[0];
                chain->chain_links[cmd].argv[chain->chain_links[cmd].argc] = NULL;
                ++cmd;
            }        
            idxes1.i = -1;
        }
        ++idxes1.j;
    }
    chain->chain_links_count = cmd;
}

void recursive_args(int vector_for_pipes[MAX_CHAIN_LINKS_COUNT][MATRIX_SIZE], chain_t* chain, int i) {
    int pid = fork();
    if (i == chain->chain_links_count) {
        return;
    }
    if (pid == 0) {
        if (i != 0) {
            dup2(vector_for_pipes[i - 1][0], STDIN_FILENO);
        }
        if (i != chain->chain_links_count - 1) {
            dup2(vector_for_pipes[i][1], STDOUT_FILENO);
        }
        for (int i = 0; i < chain->chain_links_count - 1; ++i) {
            close(vector_for_pipes[i][0]);
            close(vector_for_pipes[i][1]);
        }
        execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
    } else {
        if (i == chain->chain_links_count - 1) {
            for (int idx = 0; idx < chain->chain_links_count; ++idx) {
                close(vector_for_pipes[idx][0]);
                close(vector_for_pipes[idx][1]);
            }
        }
        recursive_args(vector_for_pipes, chain, i + 1);
    }
}

void run_chain(chain_t* chain) {
    auto current_chain_number = chain->chain_links_count - 1;
    int vector_for_pipes[MAX_CHAIN_LINKS_COUNT][MATRIX_SIZE];
    for (int i = 0; i < current_chain_number; ++i) {
        pipe(vector_for_pipes[i]);
    }
    recursive_args(vector_for_pipes, chain, 0);
}

int main(int argc, char* argv[]) {
    chain_t chain;

    create_chain(argv[1], &chain);
    run_chain(&chain);

    return 0;
}