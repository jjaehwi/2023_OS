#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
int number = 0; // 일반출력변수
void *sem_snd(void *arg);
void *sem_rev(void *arg);
int main()
{
    int state = 0;
    pthread_t t1, t2, t3; // 3개의 스레드 선언
    char thread1[] = "Thread_A";
    char thread2[] = "Thread_B";
    char thread3[] = "Thread_C";
    void *result_t = NULL;
    pthread_create(&t1, NULL, sem_snd, thread1); // 쓰레드 1 생성 (sem_snd 호출)
    pthread_create(&t2, NULL, sem_rev, thread2); // 쓰레드 2 생성 (sem_rev 호출)
    pthread_create(&t3, NULL, sem_rev, thread3); // 스레드 3 생성 (sem_rev 호출)
    pthread_join(t1, &result_t);                 // 쓰레드가 종료되기를 기다린 후 메인 프로세스가 종료되도록
    pthread_join(t2, &result_t);                 // pthread_join 을 호출
    pthread_join(t3, &result_t);                 // 그 결과는 3개의 쓰레드 동작이 모두 종료되고 메인 프로세스가 종료된다.
    printf("Finale Number : %d\n", number);
    return 0;
}
void *sem_snd(void *arg)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 1E6; j++)
            number++;
        printf("%s Running : %d\n", (char *)arg, number);
        sleep(0.1);
    }
}
void *sem_rev(void *arg)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 1E6; j++)
            number--;
        printf("%s Running : %d\n", (char *)arg, number);
        sleep(0.1);
    }
}
