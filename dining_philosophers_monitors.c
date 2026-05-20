#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N        5
#define LEFT(i)  ((i + N - 1) % N)
#define RIGHT(i) ((i + 1) % N)
#define MEALS    3

typedef enum { THINKING, HUNGRY, EATING } State;

static State           state[N];
static pthread_mutex_t monitor_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  self[N];

static void test(int i)
{
    if (state[i] == HUNGRY &&
        state[LEFT(i)]  != EATING &&
        state[RIGHT(i)] != EATING)
    {
        state[i] = EATING;
        pthread_cond_signal(&self[i]);
    }
}

static void pickup(int i)
{
    pthread_mutex_lock(&monitor_lock);
    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);
    test(i);
    if (state[i] != EATING)
        pthread_cond_wait(&self[i], &monitor_lock);
    pthread_mutex_unlock(&monitor_lock);
}

static void putdown(int i)
{
    pthread_mutex_lock(&monitor_lock);
    state[i] = THINKING;
    printf("Philosopher %d puts down forks\n", i);
    test(LEFT(i));
    test(RIGHT(i));
    pthread_mutex_unlock(&monitor_lock);
}

static void *philosopher(void *arg)
{
    int id = *(int *)arg;
    for (int meal = 0; meal < MEALS; meal++) {
        printf("Philosopher %d is thinking\n", id);
        sleep(1 + rand() % 2);
        pickup(id);
        printf("Philosopher %d is EATING (meal %d/%d)\n", id, meal + 1, MEALS);
        sleep(1 + rand() % 2);
        putdown(id);
    }
    printf("Philosopher %d is done.\n", id);
    return NULL;
}

int main(void)
{
    pthread_t tid[N];
    int       ids[N];

    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        pthread_cond_init(&self[i], NULL);
    }
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(tid[i], NULL);

    for (int i = 0; i < N; i++)
        pthread_cond_destroy(&self[i]);
    pthread_mutex_destroy(&monitor_lock);

    puts("All philosophers are done.");
    return 0;
}
