#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int* chairs;
int* results;
int num_users;
int empty_chairs_counter = 0;
pthread_mutex_t lock;

void shuffle(int* array, int size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void* runner(void* tid) {
    int thread_id = *(int*)tid;
    
    while (1) {
        int random_chair = rand() % num_users; // Generate random chair index (0 to num_users - 1)

        pthread_mutex_lock(&lock); // Lock the mutex

        if (chairs[random_chair] == 0) {
            chairs[random_chair] = thread_id;
            results[thread_id] = random_chair;

            empty_chairs_counter++;

            printf("Remaining chairs: %d\n", num_users - empty_chairs_counter);

            pthread_mutex_unlock(&lock); // Unlock the mutex

            break;
        }
        
        pthread_mutex_unlock(&lock); // Unlock the mutex
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <num_users>\n", argv[0]);
        return 1;
    }

    num_users = atoi(argv[1]);

    srand(time(NULL));

    pthread_t* threads = (pthread_t*)malloc((num_users + 1) * sizeof(pthread_t));
    int* thread_ids = (int*)malloc((num_users + 1) * sizeof(int));
    chairs = (int*)calloc(num_users, sizeof(int));
    results = (int*)calloc(num_users + 1, sizeof(int));

    pthread_mutex_init(&lock, NULL);

    for (int i = 1; i <= num_users; i++) {
        thread_ids[i] = i;
    }

    shuffle(thread_ids + 1, num_users);

    for (int i = 1; i <= num_users; i++) {
        pthread_create(&threads[i], NULL, runner, &thread_ids[i]);
    }

    for (int i = 1; i <= num_users; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    for (int i = 1; i <= num_users; i++) {
        if (results[i] == 0) {
            printf("User %d couldn't find an empty chair.\n", i);
        } else {
            printf("User %d sat in chair %d.\n", i, results[i]);
        }
    }

    free(threads);
    free(thread_ids);
    free(chairs);
    free(results);

    return 0;
}
