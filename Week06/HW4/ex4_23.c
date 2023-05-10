#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_primes(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s [number]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    pthread_t tid;
    pthread_create(&tid, NULL, print_primes, &n);
    pthread_join(tid, NULL);

    return 0;
}

void *print_primes(void *arg)
{
    int n = *((int *)arg);
    printf("Prime numbers less than or equal to %d:\n", n);

    // Check for prime numbers
    for (int i = 2; i <= n; i++)
    {
        int is_prime = 1;
        for (int j = 2; j <= i / 2; j++)
        {
            if (i % j == 0)
            {
                is_prime = 0;
                break;
            }
        }
        if (is_prime)
        {
            printf("%d ", i);
        }
    }

    printf("\n");
    pthread_exit(NULL);
}
