#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>

struct CmdOpts {
  uint64_t k;
  uint64_t mod;
  char servers[255];
};

struct Server {
  char ip[255];
  int port;
};

struct ServerArray {
  struct Server* servers;
  int length;
};

struct DoreqArgs {
  struct ServerArray serv_array;
  uint64_t k;
  uint64_t mod;
  int i;
};

struct CmdOpts parseOpts(int argc, char **argv);
struct ServerArray parse_servers(char* filename);
int count_lines(FILE* file);
void* ThreadDoreq(void *args);
void doreq(struct DoreqArgs* args);

uint64_t total = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }

  return result % mod;
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  struct CmdOpts cmd_opts = parseOpts(argc, argv);

  uint64_t k = cmd_opts.k;
  uint64_t mod = cmd_opts.mod;
  char servers[255]; // On Linux the maximum length for a file name is 255 bytes
  memcpy(&servers, cmd_opts.servers, strlen(cmd_opts.servers));

  struct ServerArray servers_array = parse_servers(servers);
  unsigned int servers_num = servers_array.length;

  struct Server *to = malloc(sizeof(struct Server) * servers_num);

  for (int i = 0; i < servers_array.length; i++) {
    to[i].port = servers_array.servers[i].port;
    memcpy(to[i].ip, servers_array.servers[i].ip, sizeof(servers_array.servers[i].ip)); 
  }

  pthread_t threads[servers_num];
  struct DoreqArgs args[servers_num];
  for (int i = 0; i < servers_num; i++) {
    args[i].serv_array = servers_array;
    args[i].i = i;
    args[i].k = k;
    args[i].mod = mod;

    if (pthread_create(&threads[i], NULL, ThreadDoreq, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  for (int i = 0; i < servers_num; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("answer: %llu\n", total);
  free(to);
  return 0;
}

void* ThreadDoreq(void *args) {
  struct DoreqArgs *dargs = (struct DoreqArgs *)args;
  doreq(dargs);
  return NULL;
}

void doreq(struct DoreqArgs* args) {
  int i = args->i;
  uint64_t k = args->k;
  uint64_t mod = args->mod;
  int servers_num = args->serv_array.length;
  struct Server to = args->serv_array.servers[i];

  printf("req to %s:%d\n", to.ip, to.port);

  struct hostent *hostname = gethostbyname(to.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", to.ip);
    exit(1);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(to.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    exit(1);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  uint64_t begin = i*k / servers_num + 1;
  uint64_t end = i == (servers_num - 1) ? k + 1 : (i+1)*k/servers_num + 1;

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));

  pthread_mutex_lock(&mut);
  total *= answer;
  pthread_mutex_unlock(&mut);

  close(sck);
}

struct ServerArray parse_servers(char filename[255]) {
  if (access(filename, F_OK) == -1) {
    printf("file \"%s\" does not exist\n", filename);
    exit(0);
  }

  FILE* file = fopen(filename, "r");
  if (!file) {
    printf("cannot open file %s\n", filename);
    exit(0);
  }

  int servers_count = count_lines(file);
  struct Server servers[servers_count];

  char str[260];
  char* pch;
  for (int i = 0; i < servers_count; i++) {
    fscanf(file, "%s", str);
    strcpy(servers[i].ip, strtok(str, ":"));
    servers[i].port = atoi(strtok(NULL, ":"));
  }
  fclose(file);
  
  struct ServerArray serv_arr = {servers, servers_count};
  return serv_arr;
}

int count_lines(FILE* file) {
  if (file == NULL) return 0;

  int lines = 0;
  char last = '\n';
  
  for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
    if (c == '\n') lines++;
    last = c;
  }
  if (last != '\n') lines++;

  fseek(file, 0, SEEK_SET); 
  return lines;
}

// CMD OPTIONS

struct CmdOpts parseOpts(int argc, char **argv) {
  struct CmdOpts cmd_opts = {
    -1, 
    -1, 
    '\0' // On Linux the maximum length for a file name is 255 bytes
  };

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            if (!ConvertStringToUI64(optarg, &cmd_opts.k)) {
              printf("k: failed to convert string to uint64_t\n");
              exit(1);
            }
            break;
          case 1:
            cmd_opts.mod = atoi(optarg);
            if (cmd_opts.mod <= 0) {
              printf("mod: failed to convert string to uint64_t\n");
              exit(1);
            }
            break;
          case 2:
            memcpy(cmd_opts.servers, optarg, strlen(optarg));
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;

      case '?':
        printf("Arguments error\n");
        break;

      default:
        fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    exit(1);
  }

  if (cmd_opts.k == -1 || cmd_opts.mod == -1 || !strlen(cmd_opts.servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    exit(1);
  }

  return cmd_opts;
}
