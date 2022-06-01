#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

#define SADDR struct sockaddr

struct CmdOpts {
  int bufsize;
  int serv_port;
};

struct CmdOpts parseOpts(int argc, char **argv);

int main(int argc, char *argv[]) {
  const size_t kSize = sizeof(struct sockaddr_in);

  struct CmdOpts cmd_opts = parseOpts(argc, argv);

  int lfd, cfd;
  int nread;
  char buf[cmd_opts.bufsize];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, kSize);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(cmd_opts.serv_port);

  if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(lfd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  while (1) {
    unsigned int clilen = kSize;

    if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
      perror("accept");
      exit(1);
    }
    printf("connection established\n");

    while ((nread = read(cfd, buf, cmd_opts.bufsize)) > 0) {
      write(1, &buf, nread);
    }

    if (nread == -1) {
      perror("read");
      exit(1);
    }
    close(cfd);
  }
}

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {100, 10050}; // default value
  int bufsize;
  int port;

  while (true) {
    static struct option options[] = {{"bufsize", required_argument, 0, 0},
                                      {"serv_port", required_argument, 0, 0},
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
          case 1:
            port = atoi(optarg);
            if (port < 1024 || port > 65535) {
              printf("port number should be in range [1024; 65535]");
              exit(1);
            }
            cmd_opts.serv_port = port;
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
    }
  }
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--serv_port")) {
      shift_opt(&argc, i, argv);
    }
  }

  return cmd_opts;
}