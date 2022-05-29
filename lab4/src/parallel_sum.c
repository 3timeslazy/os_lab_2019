#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <getopt.h>

#include <pthread.h>

#include "parallel_sum.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs *args) {
  int sum = 0;
  // TODO: your code here 
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  struct CmdOpts cmd_opts = parseOpts(argc, argv);
  // printf("threads_num: %d, array_msize: %d, seed: %d\n", cmd_opts.threads_num, cmd_opts.array_size, cmd_opts.seed);

  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;
  pthread_t threads[threads_num];

  /*
   * TODO:
   * your code here
   * Generate array here
   */

  int *array = malloc(sizeof(int) * array_size);

  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {-1, -1, -1};

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            cmd_opts.seed = atoi(optarg);
            if (cmd_opts.seed <= 0) {
              printf("seed must be a positive number\n");
              exit(1);
            }
            break;
          case 1:
            cmd_opts.array_size = atoi(optarg);
            if (cmd_opts.array_size <= 0) {
              printf("array_size must be a positive number\n");
              exit(1);
            }
            break;
          case 2:
            cmd_opts.threads_num = atoi(optarg);
            if (cmd_opts.threads_num <= 0) {
              printf("threads_num must be a positive number\n");
              exit(1);
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    exit(1);
  }

  if (cmd_opts.seed == -1 || cmd_opts.array_size == -1 || cmd_opts.threads_num == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    exit(1);
  }

  return cmd_opts;
}