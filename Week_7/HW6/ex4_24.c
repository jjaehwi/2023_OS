#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <asm/unistd.h>

#define NUM_THREADS 4

// 원에 생성되는 점의 개수를 공통자원(전역변수)으로 설정
int totalCirclePoint = 0;
// 변수에 대한 동시 접근을 제어하기 위해 sem 세마포어를 사용
// sem_t sem;

// 변수에 대한 동시 접근을 제어하기 위해 mutex 뮤텍스를 사용
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *monteCarlo(void *arg)
{
    int generatedPoint = *(int *)arg;
    int tempCirclePoint = 0;

    // time(NULL)과 현재 스레드의 ID 값을 XOR 연산하여 시드 값을 생성
    // 각 스레드마다 다른 시드 값을 가지도록 하여 스레드 간에 생성되는 난수 값이 다르게 설정
    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)pthread_self();
    for (int i = 0; i < generatedPoint; i++)
    {
        // rand_r() 함수를 사용하여 0에서 1 사이의 랜덤한 값을 생성
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        // 생성된 점이 원 안에 위치하면 tempCirclePoint 값을 1 증가
        if (x * x + y * y <= 1)
            tempCirclePoint++;
    }

    // wait 함수를 통해 세마포어 값이 0 인지 아닌지를 체크
    // 값이 0 인 경우 그 값이 양수가 될 때 까지 공통 자원이 있는 섹션에 들어가려고 했던 스레드는 대기상태가 됨
    // 0 이 아닌 경우 접근가능
    // sem_wait(&sem);

    // mutex lock을 사용하여 공유 변수에 대한 접근 제어
    pthread_mutex_lock(&mutex);

    // 각 스레드에서 생성된 tempCirclePoint 값을 전체 스레드의 totalCirclePoint 변수에 더함
    totalCirclePoint += tempCirclePoint;

    // 나갈 때는 0 이 아닌 값으로 바꿔주고 나가야 다른 스레드들이 접근할 수 있다.
    // sem_post(&sem);

    // mutex unlock을 사용하여 공유 변수의 접근 제어 해제
    pthread_mutex_unlock(&mutex);

    printf("PID (Process ID) : %d\n", (int)getpid());
    printf("TID (Thread  ID) : %d\n", (int)syscall(__NR_gettid));
    printf("Generated %d points, %d points are in the circle.\n", generatedPoint, tempCirclePoint);
    printf("----------------------------------------------------------\n");
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int n;
    srand((unsigned)time(NULL));
    pthread_t threads[NUM_THREADS];
    int generatedPoint[NUM_THREADS];

    if (argc == 1)
        n = 1000000;
    else
        n = atoi(argv[1]);

    // 세마포어 생성
    // sem_init(&sem, 0, 1);

    // mutex 초기화
    pthread_mutex_init(&mutex, NULL);

    int totalPoint = 0;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        // 스레드 당 1000000 개의 점을 생성하고 totalPoint 에 추가
        generatedPoint[i] = n;
        totalPoint += generatedPoint[i];
        // 스레드를 생성하고 스레드에서는 1000000 을 매개변수로 가지는 monteCarlo 함수를 실행한다.
        pthread_create(&threads[i], NULL, monteCarlo, (void *)&generatedPoint[i]);
    }

    // 각 스레드들이 끝날 때까지 기다렸다가 최종 계산을 하고 출력을 하도록 한다.
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("totalCirclePoint: %d, totalPoint: %d\n", totalCirclePoint, totalPoint);
    double pi = 4 * (double)totalCirclePoint / totalPoint;
    printf("PI = %f\n", pi);

    // sem_destroy(&sem);

    // 뮤텍스 제거
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);
}