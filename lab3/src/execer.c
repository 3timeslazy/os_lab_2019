#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define SEQUENTIAL "sequential_min_max"

int main(int argc, char* argv[]) {
	if (access(SEQUENTIAL, F_OK) == -1) {
		printf("Error: " SEQUENTIAL " not found\n");
		return -1;
	}
    
    int pid = fork();
    if (pid == 0) {
        char seed[50];
        sprintf(seed, "%d", rand());
        char *args[4] = {SEQUENTIAL, seed, "1000", NULL};

        execvp("./"SEQUENTIAL, args);
    }
    if (pid < 0) {
        printf("fork failed!\n");
        return 1;
    }
    
    wait(NULL);
    return 0;
}