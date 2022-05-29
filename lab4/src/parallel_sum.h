#ifndef PARALLEL_SUM_H
#define PARALLEL_SUM_H

#include "utils.h"

struct CmdOpts {
  int array_size;
  int seed;
  int threads_num;
};

struct CmdOpts parseOpts(int argc, char **argv);

#endif
