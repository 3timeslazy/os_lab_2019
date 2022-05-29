#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main ()
{
    pid_t child_pid;

    child_pid = fork();
    if (child_pid < 0) {
      printf("Fork failed!\n");
      return 1;
    }
    printf("successful fork\n");

    if (child_pid == 0) {
        printf("child process done\n");
        return 0;
    }

    printf("zombie process PID: %d\n", child_pid);
    printf("You can find zombie with command \"ps aux | grep Z\"\n");
    sleep(60);
    return 0;
}