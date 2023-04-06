#include "calc_times.h"
#include <asm/unistd.h>

// Variables for clock and time
struct tms tmsstart, tmsend;
clock_t start, end;
// Variables for result and computation storage
unsigned int sum = 0;
struct range
{
    int start;
    int end;
};

// Function 'runner' Declation
void *runner(void *p);

// Main Function
//  argument: 4(default), 3,2,1(75%, 50%, 25% of full calculation)
int main(int argc, char **argv)
{
    struct range A;

    int n;
    if (argc == 1)
        n = 4;
    else
        n = atoi(argv[1]);
    // Set-up parameters to argumnert of each thread
    // The first and final number for addition
    A.start = 1;
    A.end = n;

    // Start time of calculation
    start = times(&tmsstart);

    runner(&A);

    // End time of Calculation
    end = times(&tmsend);
    pr_times(end - start, &tmsstart, &tmsend);

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

    for (k = q->start; k <= (q->end) * 4; k++)
        for (j = 0; j < 10000; j++)
        {
            tmp += j;
            for (i = 0; i < 10000; i++)
                if (j % 2 == 0)
                    tmp += i;
                else
                    tmp -= i;
        }
    printf("Function Sum Result = %d\n", tmp);
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
}
