#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    pid_t pid_1;
    pid_t pid_2;
    pid_t pid_3;

    // fork child-1
    pid_1 = fork();
    // Check the result of fork
    if (pid_1 < 0)
    { // When fork child-1 failed
        fprintf(stderr, "Fork failed");
        return 1;
    }
    else if (pid_1 == 0)
    { // Code of Child-1
        while (1)
        {
            printf("This is Child-1 Process, PID : %d, Parent PID: %d\n",
                   (int)getpid(), (int)getppid());
            sleep(5);
        }
    }
    else
    {
        pid_3 = fork(); // fork child-2
        if (pid_3 < 0)
        {
            fprintf(stderr, "Fork failed");
            return 1;
        }
        else if (pid_3 == 0)
        {                   // Code of Child-2
            pid_2 = fork(); // fork child-child
            if (pid_2 < 0)
            {
                fprintf(stderr, "Fork failed");
                return 1;
            }
            else if (pid_2 == 0)
            { // Code of Child-Child
                while (1)
                {
                    printf("This is Child-Child Process, PID : %d, Parent PID:%d\n",
                           (int)getpid(), (int)getppid());
                    sleep(5);
                }
            }
            else
            {
                while (1)
                {
                    printf("This is Child-2 Process, PID : %d, Parent PID: %d\n",
                           (int)getpid(), (int)getppid());
                    sleep(5);
                }
            }
        }
        else
        {
            while (1)
            {
                printf("This is Parent Process, PID : %d, Parent PID: %d\n",
                       (int)getpid(), (int)getppid());
                sleep(5);
            }
        }
    }
    return 0;
}
