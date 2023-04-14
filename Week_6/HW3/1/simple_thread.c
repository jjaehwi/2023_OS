#include <stdio.h>
#include <pthread.h>
#include <asm/unistd.h>
#include <unistd.h>

// The number of Thread
#define THREAD_NUM 2

// Function 'runner' Declation
void *runner();

// Main Function
int main(int argc, char **argv)
{
    pthread_t tid[THREAD_NUM];
    pthread_attr_t attr;

    setbuf(stdout, NULL);

    pthread_attr_init(&attr);

    // Thread Creation
    pthread_create(&tid[0], &attr, runner, NULL);
    pthread_create(&tid[1], &attr, runner, NULL);

    // Thread Synchronization Code
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}

// The function which will be run by thread
void *runner()
{
    printf("******************************************************\n");
#ifdef SYS_gettid
    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("TID (Thread  ID) : %d\n", (int)syscall(SYS_gettid));
#else
    // syscall(__NR_gettid) is used due to unavailable of gettid() function in unistd.h
    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("TID (Thread  ID) : %d\n", (int)syscall(__NR_gettid));
#endif
    sleep(10);
    printf("******************************************************\n");

    pthread_exit(0);
}
