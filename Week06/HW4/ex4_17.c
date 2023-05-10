#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *t_function(void *data)
{
    printf("Thread Start\n");
    sleep(100);
    printf("Thread end\n");
}

int main()
{
    pthread_t p_thread;

    pid_t pid;
    pid = fork();
    if (pid == 0)
    { /* child process */
        fork();
        pthread_create(&p_thread, NULL, t_function, (void *)NULL);
    }
    fork();

    printf("forked\n");

    sleep(100);
    return 0;
}