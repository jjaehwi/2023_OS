#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork failed");
        return -1;
    }
    else if (pid == 0)
    {
        printf("This is Child process, PID : %d, Parent PID: %d\n",
               (int)getpid(), (int)getppid());
        sleep(10);
        exit(3);
    }
    else
    {
        printf("This is parent process, PID :%d, Parent PID: %d\n",
               (int)getpid(), (int)getppid());
        wait(&status);
    }

    return 0;
}
