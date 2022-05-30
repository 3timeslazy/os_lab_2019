#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdopts.h"

struct CalcFactorialArgs {
    int begin;
    int end;
};
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
long long factorial = 1;
void calc_factorial(struct CalcFactorialArgs* args);

int main(int argc, char **argv) {
    struct CmdOpts cmd_opts = parseOpts(argc, argv);
    if (cmd_opts.pnum > cmd_opts.k) cmd_opts.pnum = cmd_opts.k;

    printf("k=%d mod=%d pnum=%d\n", cmd_opts.k, cmd_opts.mod, cmd_opts.pnum);

    pthread_t threads[cmd_opts.pnum];
    struct CalcFactorialArgs args[cmd_opts.pnum];

    int chunk_size = cmd_opts.k / cmd_opts.pnum;
    int thidx = 0;

    for (int i = 1; i < cmd_opts.k; i+=chunk_size) {
        struct CalcFactorialArgs arg = {
            i, i + chunk_size
        };
        if (arg.end > cmd_opts.k) arg.end = cmd_opts.k;
        args[thidx] = arg;
        
        if (pthread_create(&threads[thidx], NULL, (void*)calc_factorial, (void*)&args[thidx])) {
            perror("pthread_create");
            exit(1);
        }
        thidx++;
    }

    printf("thdix %d\n", thidx-1);
    for (int i = 0; i < cmd_opts.pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    printf("%d! mod %d = %lld\n", cmd_opts.k, cmd_opts.mod, factorial % cmd_opts.mod);
    return 0;
}

void calc_factorial(struct CalcFactorialArgs* args) {
    long long p = 1;
    for (int i = args->begin+1; i <= args->end; i++) {
        p *= i;
    };
    
    pthread_mutex_lock(&mut);
    factorial *= p;
    pthread_mutex_unlock(&mut);
}