#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
    pid_t pid, pid1;
    int i;
    for (i = 0; i < 4; i++)
    {
        pid = fork();
    }
    if (pid < 0)
    {
        /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0)
    {
        /* child process */
        printf("child: pid = %d\n", pid);
    }
    else
    {
        /* parent process */
        printf("parent: pid = %d\n", pid);
    }
    sleep(10);
    return 0;
}
