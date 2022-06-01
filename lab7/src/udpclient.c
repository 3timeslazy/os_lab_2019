#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
  char sendline[cmd_opts.bufsize], recvline[cmd_opts.bufsize + 1];
  struct sockaddr_in servaddr;
  struct sockaddr_in cliaddr;

  if (argc != 2) {
    printf("usage: client <IPaddress of server>\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(cmd_opts.serv_port);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0) {
    perror("inet_pton problem");
    exit(1);
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket problem");
    exit(1);
  }

  write(1, "Enter string\n", 13);

  while ((n = read(0, sendline, cmd_opts.bufsize)) > 0) {
    if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) {
      perror("sendto problem");
      exit(1);
    }

    if (recvfrom(sockfd, recvline, cmd_opts.bufsize, 0, NULL, NULL) == -1) {
      perror("recvfrom problem");
      exit(1);
    }

    printf("REPLY FROM SERVER= %s\n", recvline);
  }
  close(sockfd);
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
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--serv_port")) {
      shift_opt(&argc, i, argv);
      break;
    }
  }

  return cmd_opts;
}
