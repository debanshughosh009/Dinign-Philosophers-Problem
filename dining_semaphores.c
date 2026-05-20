#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N        5
#define LEFT(i)  ((i + N - 1) % N)
#define RIGHT(i) ((i + 1) % N)
#define MEALS    3

typedef enum { THINKING, HUNGRY, EATING } State;

static State state[N];
static sem_t mutex;
static sem_t self[N];

static void test(int i)
{
    if (state[i] == HUNGRY &&
        state[LEFT(i)]  != EATING &&
        state[RIGHT(i)] != EATING)
    {
        state[i] = EATING;
        sem_post(&self[i]);
    }
}

static void pickup(int i)
{
    sem_wait(&mutex);
    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);
    test(i);
    sem_post(&mutex);
    sem_wait(&self[i]);
}

static void putdown(int i)
{
    sem_wait(&mutex);
    state[i] = THINKING;
    printf("Philosopher %d puts down forks\n", i);
    test(LEFT(i));
    test(RIGHT(i));
    sem_post(&mutex);
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

    sem_init(&mutex, 0, 1);
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        sem_init(&self[i], 0, 0);
    }
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&tid[i], NULL, philosopher, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(tid[i], NULL);

    sem_destroy(&mutex);
    for (int i = 0; i < N; i++)
        sem_destroy(&self[i]);

    puts("All philosophers are done.");
    return 0;
}
