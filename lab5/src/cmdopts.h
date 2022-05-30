#ifndef CMD_OPTS_H
#define CMD_OPTS_H

struct CmdOpts {
  int k;
  int pnum;
  int mod;
};

struct CmdOpts parseOpts(int argc, char **argv);

#endif
