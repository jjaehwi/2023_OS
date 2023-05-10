#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// Function for Exact time calculation
// Calculate Time using system clock tick & time per tick(clktck)
static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
	static long clktck = 0;
	if (clktck == 0)
	{
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
		{
			printf("Sysconf Error\n");
			exit(0);
		}
	}
	printf("Real CPU Time : %7.10f\n", real / (double)clktck);
	printf("User CPU Time : %7.10f\n", (tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
	printf("Sys  CPU Time : %7.10f\n", (tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
}
