#include <pthread.h>
#include "calc_times.h"
#include <asm/unistd.h>

// The number of Thread
#define THREAD_NUM 4

// Variables for clock and time
struct tms tmsstart, tmsend;
clock_t start, end;

// Variables for result and computation storage
unsigned int sum = 0;

unsigned int psum[THREAD_NUM];

struct range
{
    int start;
    int end;
    int flag;
};

// Function 'runner' Declation
void *runner(void *p);

// Main Function
//  argument: 4(default), 3,2,1(75%, 50%, 25% of full calculation)
int main(int argc, char **argv)
{
    pthread_t tid[THREAD_NUM];
    pthread_attr_t attr;

    int i;
    struct range A[THREAD_NUM];
    int n;

    if (argc == 1)
        n = 4;
    else
        n = atoi(argv[1]);

    pthread_attr_init(&attr);

    // Set-up parameters to argumnert of each thread
    // The first and final number for addition
    for (i = 0; i < THREAD_NUM; i++)
    {
        A[i].start = i * n + 1;
        A[i].end = (i + 1) * n;
        A[i].flag = i + 1;
        psum[i] = 0;
    }

    // Start time of calculation
    start = times(&tmsstart);

    // Thread Creation
    for (i = 0; i < THREAD_NUM; i = i + 1)
    {
        pthread_create(&tid[i], &attr, runner, &A[i]);
    }
    // Thread Synchronization Code
    for (i = 0; i < THREAD_NUM; i = i + 1)
    {
        pthread_join(tid[i], NULL);
    }

    for (i = 0; i < THREAD_NUM; i = i + 1)
    {
        sum += psum[i];
    }

    // End time of Calculation
    end = times(&tmsend);
    pr_times(end - start, &tmsstart, &tmsend);
    printf("Result : %d\n", sum);

    return 0;
}

// The function which will be run by thread
void *runner(void *p)
{
    int i, j, k;
    unsigned int tmp;
    struct range *q;
    q = p;
    tmp = 0;
    for (k = q->start; k <= (q->end); k++)
        for (j = 0; j < 10000; j++)
        {
            tmp += j;
            for (i = 0; i < 10000; i++)
                if (j % 2 == 0)
                    tmp += i;
                else
                    tmp -= i;
        }

    for (i = 0; i < THREAD_NUM; i++)
    {
        if (q->flag == (i + 1))
            psum[i] = tmp;
    }

    printf("Thread Sum Result = %d\n", tmp);
    printf("******************************************************\n");
#ifdef SYS_gettid
    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("TID (Thread  ID) : %d\n", (int)syscall(SYS_gettid));
#else
    // syscall(__NR_gettid) is used due to unavailability of gettid() function in unistd.h
    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("TID (Thread  ID) : %d\n", (int)syscall(__NR_gettid));
#endif
    printf("******************************************************\n");

    pthread_exit(0);
}
