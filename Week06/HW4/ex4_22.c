#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int sum = 0;
int num;
int numCount;
int max = -100000;
int min = 100000;
int i;
float avg;

void *avg_runner(int arr[])
{
    int i;
    for (i = 0; i < numCount; i++)
    {
        sum += arr[i];
        avg = sum / numCount;
    }
    pthread_exit(0);
}

void *min_runner(int arr[])
{
    int i;
    for (i = 0; i < numCount; i++)
    {
        if (i == 0)
        {
            min = arr[i];
        }
        else if (min > arr[i])
            min = arr[i];
    }
    pthread_exit(0);
}

void *max_runner(int arr[])
{
    int i;
    for (i = 0; i < numCount; i++)
    {
        if (i == 0)
        {
            max = arr[i];
        }
        else if (max < arr[i])
            max = arr[i];
    }
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    printf("This program finds the maximum, minimum, and average of a series of numbers.\n");
    printf("How many numbers would you like to process?\n");
    scanf("%d", &numCount);
    int arr[numCount], i; // array declared to hold numbers
    printf("Enter the numbers\n");
    for (i = 0; i < numCount; i++)
    {
        // reading array values in main itself
        scanf("%d", &arr[i]);
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr); // creating threads
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_create(&thread1, &attr, avg_runner, arr); // threads callling
    pthread_create(&thread2, &attr, min_runner, arr);
    pthread_create(&thread3, &attr, max_runner, arr);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    // printing results
    printf("The average is: %f\n", avg);
    printf("The minimum is: %d\n", min);
    printf("The maximum is: %d\n", max);
    return 0;
}
