#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>

#include <getopt.h>

#include <pthread.h>

#include "parallel_sum.h"
#include "utils.h"
#include "libsum/sum.h"

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  struct CmdOpts cmd_opts = parseOpts(argc, argv);
  
  uint32_t threads_num = cmd_opts.threads_num;
  uint32_t array_size = cmd_opts.array_size;
  uint32_t seed = cmd_opts.seed;
  pthread_t threads[threads_num];

  threads_num = threads_num > array_size ? array_size : threads_num;

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int chunk_size = array_size / threads_num;

  struct SumArgs args[threads_num];
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = i * chunk_size + chunk_size;
    if (args[i].end > array_size) { args[i].end = array_size; }

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  unsigned long long total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  printf("Total: %llu\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
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