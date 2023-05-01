#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
void Error(const char *mes);
int number = 0; // 일반출력변수
sem_t bin_sem;  // 세마포어 생성
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
    /*
     * sem_init(sem_t *sem, int pshared, unsigned int value);
     *
     * sem가 가리키는 세마포어 객체를 초기화 한다.
     * 아래 초기화문에서는 세번째 인자 1로 객체가 초기화 되며 두번째 인자 0은
     * 세마포어가 현재 프로세스에서만 사용될지(현재 프로세스에서만 사용 = 0)
     * 아니면 여러 프로세스간에 공유될지 지정한다.
     * LinuxThreads는 세마포어가 여러 프로세스 간에 공유되는 것을 지원하지 않기때문에
     * 두번째 인자가 0이 아닐 경우 sem_init 는 항상 ENOSYS 에러코드를 반환한다.
     */
    state = sem_init(&bin_sem, 0, 1); // 최초 1로 세팅
    if (state != 0)
        Error("sem_init Error\n");
    pthread_create(&t1, NULL, sem_snd, thread1); // 쓰레드 1 생성 (sam_snd 호출)
    pthread_create(&t2, NULL, sem_rev, thread2); // 쓰레드 2 생성 (sam_rev 호출)
    pthread_create(&t3, NULL, sem_rev, thread3); // 스레드 3 생성 (sam_rev 호출)
    pthread_join(t1, &result_t);                 // 쓰레드가 종료되기를 기다린 후 메인 프로세스가 종료되도록
    pthread_join(t2, &result_t);                 // pthread_join 을 호출
    pthread_join(t3, &result_t);                 // 그 결과는 3개의 쓰레드 동작이 모두 종료되고 메인 프로세스가 종료된다.
    printf("Finale Number : %d\n", number);
    /*
     * int sem_destroy(sem_t *sem);
     *
     * 세마포어와 이에 관련된 리소스들을 소멸시킨다.
     */
    sem_destroy(&bin_sem);
    return 0;
}
void *sem_snd(void *arg)
{
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        /*
         * int sem_wait(sem_t *sem);
         *
         * 세마포어 값을 하나 감소시킨다
         * 값이 0인 경우에는 1 이상이 될 때까지 대기 상태에 있다가
         * 1이상이 되면 값을 감소시키며 대기상태에서 빠져나오게 된다.
         * 즉 세마포어는 0보다 작은 값이 될 수 없다.
         */
        sem_wait(&bin_sem);
        number++;
        printf("%s Running : %d\n", (char *)arg, number);
        /*
         * int sem_post(sem_t *sem);
         * 세마포어의 값을 하나 증가시킨다.
         */
        sem_post(&bin_sem);
        sleep(0.1);
    }
}
void *sem_rev(void *arg)
{
    int i = 0;
    for (i = 0; i < 2; i++)
    {
        sem_wait(&bin_sem);
        number--;
        printf("%s Running : %d\n", (char *)arg, number);
        sem_post(&bin_sem);
        sleep(0.1);
    }
}
void Error(const char *mes)
{
    printf("%s\n", mes);
    exit(0);
}
