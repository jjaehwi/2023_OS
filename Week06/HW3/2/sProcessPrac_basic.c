#include "calc_times.h"
#include <asm/unistd.h>
#include "stdlib.h"

struct tms tmsstart, tmsend;
clock_t start, end;
unsigned int sum = 0;

struct range
{
    int start;
    int end;
};

// argument: 4(default), 3,2,1(75%, 50%, 25% of full calculation)
int main(int argc, char **argv)
{
    int i, j, k;
    int n;
    if (argc == 1)
        n = 4;
    else
        n = atoi(argv[1]);
    struct range A;

    start = times(&tmsstart);
    // Calculate using Single Process
    for (k = 0; k < n * 4; k++)
        for (j = 0; j < 10000; j++)
        {
            sum += j;
            for (i = 0; i < 10000; i++)
                if (j % 2 == 0)
                    sum += i;
                else
                    sum -= i;
        }
    printf("Iteration : %d x 4E8\n", n);
    printf("Result : %d\n", sum);
    printf("****************************************************\n");
    printf("This is single process output, Process Status Information\n");
    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("****************************************************\n");

    end = times(&tmsend);
    pr_times(end - start, &tmsstart, &tmsend);

    return 0;
}
