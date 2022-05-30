#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>

#include "cmdopts.h"

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {-1, -1, -1};

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            cmd_opts.k = atoi(optarg);
            if (cmd_opts.k <= 0) {
              printf("k must be a positive number\n");
              exit(1);
            }
            break;
          case 1:
            cmd_opts.pnum = atoi(optarg);
            if (cmd_opts.pnum <= 0) {
              printf("pnum must be a positive number\n");
              exit(1);
            }
            break;
          case 2:
            cmd_opts.mod = atoi(optarg);
            if (cmd_opts.mod <= 0) {
              printf("mod must be a positive number\n");
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

  if (cmd_opts.mod == -1 || cmd_opts.k == -1 || cmd_opts.pnum == -1) {
    printf("Usage: %s --mod \"num\" --k \"num\" --pnum \"num\" \n",
           argv[0]);
    exit(1);
  }

  return cmd_opts;
}