#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>

#include "util.h"

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

struct CmdOpts {
  int bufsize;
  int serv_port;
};

struct CmdOpts parseOpts(int argc, char **argv);

int main(int argc, char **argv) {
  struct CmdOpts cmd_opts = parseOpts(argc, argv);

  int sockfd, n;
  char mesg[cmd_opts.bufsize], ipadr[16];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  memset(&servaddr, 0, SLEN);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(cmd_opts.serv_port);

  if (bind(sockfd, (SADDR *)&servaddr, SLEN) < 0) {
    perror("bind problem");
    exit(1);
  }
  printf("SERVER starts...\n");

  while (1) {
    unsigned int len = SLEN;

    if ((n = recvfrom(sockfd, mesg, cmd_opts.bufsize, 0, (SADDR *)&cliaddr, &len)) < 0) {
      perror("recvfrom");
      exit(1);
    }
    mesg[n] = 0;

    printf("REQUEST %s      FROM %s : %d\n", mesg,
           inet_ntop(AF_INET, (void *)&cliaddr.sin_addr.s_addr, ipadr, 16),
           ntohs(cliaddr.sin_port));

    if (sendto(sockfd, mesg, n, 0, (SADDR *)&cliaddr, len) < 0) {
      perror("sendto");
      exit(1);
    }
  }
}

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {1024, 20001}; // default value
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
    if (!strcmp(argv[i], "--serv_port")) {
      shift_opt(&argc, i, argv);
      break;
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
