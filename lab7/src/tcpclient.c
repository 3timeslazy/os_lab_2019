#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>

#include "util.h"

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

struct CmdOpts {
  int bufsize;
};

struct CmdOpts parseOpts(int argc, char **argv);

int main(int argc, char *argv[]) {
  struct CmdOpts cmd_opts = parseOpts(argc, argv);

  int fd;
  int nread;
  char buf[cmd_opts.bufsize];
  struct sockaddr_in servaddr;
  if (argc < 3) {
    printf("Too few arguments \n");
    exit(1);
  }

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creating");
    exit(1);
  }

  memset(&servaddr, 0, SIZE);
  servaddr.sin_family = AF_INET;

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    perror("bad address");
    exit(1);
  }

  servaddr.sin_port = htons(atoi(argv[2]));

  if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
    perror("connect");
    exit(1);
  }

  write(1, "Input message to send\n", 22);
  while ((nread = read(0, buf, cmd_opts.bufsize)) > 0) {
    if (write(fd, buf, nread) < 0) {
      perror("write");
      exit(1);
    }
  }

  close(fd);
  exit(0);
}

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {100}; // default value
  int bufsize;

  int idx = 0;
  while (true) {
    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            bufsize = atoi(optarg);
            if (bufsize <= 0) {
              printf("bufsize must be a positive number\n");
              exit(1);
            }
            cmd_opts.bufsize = bufsize;
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

  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--bufsize")) {
      shift_opt(&argc, i, argv);
      break;
    }
  }

  return cmd_opts;
}
