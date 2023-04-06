#include <pthread.h>
#include "calc_times.h"
#include <asm/unistd.h>

// Variables for clock and time
struct tms tmsstart, tmsend;
clock_t start, end;

// Variables for result and computation storage
unsigned int sum1 = 0;
unsigned int sum2 = 0;

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
    pthread_t tid;
    pthread_attr_t attr;

    struct range A, B;
    int n, fd[2];
    unsigned int sum;

    if (argc == 1)
        n = 4;
    else
        n = atoi(argv[1]);

    pthread_attr_init(&attr);

    // Set-up parameters to argumnert of each thread
    // The first and final number for addition
    A.start = 1;
    A.end = n;
    A.flag = 0;
    B.start = 1;
    B.end = n;
    B.flag = 1;
    // Start time of calculation
    start = times(&tmsstart);

    // Process operation

    // Thread Creation
    pthread_create(&tid, &attr, runner, &A);
    runner(&B);
    // Thread Synchronization Code
    pthread_join(tid, NULL);

    sum = sum1 + sum2;
    printf("Final Result : %d\n", sum);

    // End time of Calculation
    end = times(&tmsend);
    pr_times(end - start, &tmsstart, &tmsend);

    return 0;
}

// The function which will be run by thread and process
void *runner(void *p)
{
    int i, j, k;
    unsigned int tmp;
    struct range *q;
    q = p;

    tmp = 0;

    for (k = q->start; k <= (q->end) * 2; k++)
        for (j = 0; j < 10000; j++)
        {
            tmp += j;
            for (i = 0; i < 10000; i++)
                if (j % 2 == 0)
                    tmp += i;
                else
                    tmp -= i;
        }
    switch (q->flag)
    {
    case 0:
    {
        sum1 = tmp;
        printf("Thread Sum Result = %d\n", tmp);
        break;
    }
    case 1:
    {
        sum2 = tmp;
        printf("Process Sum Result = %d\n", tmp);
        break;
    }
    default:
        printf("Thread Error with Thread < %d >\n", q->flag);
        break;
    }

    //	printf("Thread Sum Result = %d\n", tmp);
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
    switch (q->flag)
    {
    case 0:
    {
        pthread_exit(0);
        break;
    }
    case 1:
    {
        return 0;
    }
    }
}
