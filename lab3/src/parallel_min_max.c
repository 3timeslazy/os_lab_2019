#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("seed must be a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("array_size must be a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("pnum must be a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  

  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  pnum = pnum > array_size ? array_size : pnum;

  // pipes
  int results_fd[2];
  pipe(results_fd);
  int minmax_rd = results_fd[0];
  int minmax_wr = results_fd[1];

  // files
  char* fname_tmpl = "/tmp/lab3_parallel_min_max_%d";

  int chunk_size = array_size / pnum;

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid < 0) {
      printf("Fork failed!\n");
      return 1;
    }
    // successful fork
    active_child_processes += 1;

    // child process
    if (child_pid == 0) {
      int begin = i * chunk_size;
      int end = i * chunk_size + chunk_size;
      if (end > array_size) { end = array_size; }
      struct MinMax min_max = GetMinMax(array, begin, end);

      if (with_files) {
        char fname[1000];
        sprintf(fname, fname_tmpl, i);

        FILE* fptr = fopen(fname, "w");
        if (fptr == NULL) {
          printf("failed to create file!\n");
          return 1;
        }

        fprintf(fptr, "%d_%d", min_max.min, min_max.max);
        fclose(fptr);

      } else {
        write(minmax_wr, &min_max, sizeof(&min_max));
      }
  
      return 0;
    }
  }

  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }
  close(minmax_wr);

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    struct MinMax mm;

    if (with_files) {
        char fname[1000];
        sprintf(fname, fname_tmpl, i);

        FILE* fptr = fopen(fname, "r");
        if (fptr == NULL) {
          printf("failed to read file!\n");
          return 1;
        }

        fscanf(fptr, "%d_%d", &mm.min, &mm.max);
        fclose(fptr);
    } else {
      read(minmax_rd, &mm, sizeof(&mm));
    }

    if (mm.min < min_max.min) min_max.min = mm.min;
    if (mm.max > min_max.max) min_max.max = mm.max;
  }
  close(minmax_rd);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
