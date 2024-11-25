#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define N 5

sem_t chopsticks[N];

char output_buffer[N][50];
int count = 0;
pthread_mutex_t output_buffer_mutex;

void add_to_buffer(const char *output) {
    pthread_mutex_lock(&output_buffer_mutex);
    strcpy(output_buffer[count], output);
    count++;

    if (count == N) {
        for (int i = 0; i < N; i++) {
            printf("%s\n", output_buffer[i]);
        }
        printf("\n");
        count = 0;
    }
    pthread_mutex_unlock(&output_buffer_mutex);
}

void *philosopher(void *num) {
    int id = *(int *)num;
    char buf[50];
    while (1) {
        sprintf(buf, "P%d THINKING", id);
        add_to_buffer(buf);
        sleep(1);
        if (id == N - 1) {
            sprintf(buf, "P%d WAITING for right chopstick", id);
            add_to_buffer(buf);
            sem_wait(&chopsticks[(id + 1) % N]);
            sprintf(buf, "P%d picked up right chopstick", id);
            add_to_buffer(buf);
            sprintf(buf, "P%d WAITING for left chopstick", id);
            add_to_buffer(buf);
            sem_wait(&chopsticks[id]);
            sprintf(buf, "P%d picked up left chopstick", id);
            add_to_buffer(buf);
            sprintf(buf, "P%d EATING", id);
            add_to_buffer(buf);
            sleep(1);
            sem_post(&chopsticks[(id + 1) % N]);
            sprintf(buf, "P%d put down right chopstick", id);
            add_to_buffer(buf);
            sem_post(&chopsticks[id]);
            sprintf(buf, "P%d put down left chopstick", id);
            add_to_buffer(buf);
        } else {
            sprintf(buf, "P%d WAITING for left chopstick", id);
            add_to_buffer(buf);
            sem_wait(&chopsticks[id]);
            sprintf(buf, "P%d picked up left chopstick", id);
            add_to_buffer(buf);
            sprintf(buf, "P%d WAITING for right chopstick", id);
            add_to_buffer(buf);
            sem_wait(&chopsticks[(id + 1) % N]);
            sprintf(buf, "P%d picked up right chopstick", id);
            add_to_buffer(buf);
            sprintf(buf, "P%d EATING", id);
            add_to_buffer(buf);
            sleep(1);
            sem_post(&chopsticks[id]);
            sprintf(buf, "P%d put down left chopstick", id);
            add_to_buffer(buf);
            sem_post(&chopsticks[(id + 1) % N]);
            sprintf(buf, "P%d put down right chopstick", id);
            add_to_buffer(buf);
        }
    }

    return 0;
}

int main() {
    pthread_t philosophers[N];
    int ids[N];
    for (int i = 0; i < N; i++) {
        ids[i] = i;
    }

    for (int i = 0; i < N; i++) {
        sem_init(&chopsticks[i], 0, 1);
    }

    for (int i = 0; i < N; i++) {
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }
}
