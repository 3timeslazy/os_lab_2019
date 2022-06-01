#include "util.h"

void shift_opt(int* argc, int from, char **argv) {
  for (int i = from; i < *argc; i++) {
    char* tmp = argv[i];
    argv[i] = argv[i+1];
    argv[i+1] = tmp;
  }
  for (int i = from; i < *argc; i++) {
    char* tmp = argv[i];
    argv[i] = argv[i+1];
    argv[i+1] = tmp;
  }
  *argc = (*argc)-1;
}