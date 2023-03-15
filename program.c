#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>   // Add this line to include intptr_t type

#define MAX_READERS 40

int shared_resource = 0;
sem_t mutex, writeblock;
int numOfReaders, numOfWriters;

void *reader_thread(void *arg) {
    intptr_t thread_id = (intptr_t) arg;   // Replace int with intptr_t
    while (1) {
        sem_wait(&mutex);
        shared_resource++;
        if (shared_resource == 1) {
            sem_wait(&writeblock);
        }
        sem_post(&mutex);

        printf("Reader %ld: read shared resource as %d\n", thread_id, shared_resource);

        sem_wait(&mutex);
        shared_resource--;
        if (shared_resource == 0) {
            sem_post(&writeblock);
        }
        sem_post(&mutex);

        sleep(1);
    }
}

void *writer_thread(void *arg) {
    intptr_t thread_id = (intptr_t) arg;   // Replace int with intptr_t
    while (1) {
        sem_wait(&writeblock);
        shared_resource++;
        printf("Writer %ld: modified shared resource to %d\n", thread_id, shared_resource);
        sem_post(&writeblock);
        sleep(1);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <number of readers> <number of writers>\n", argv[0]);
        return 1;
    }

    numOfReaders = atoi(argv[1]);
    if (numOfReaders > MAX_READERS) {
        printf("Error: too many readers (max %d)\n", MAX_READERS);
        return 1;
    }

    numOfWriters = atoi(argv[2]);

    pthread_t readers[numOfReaders], writers[numOfWriters];
    sem_init(&mutex, 0, 1);
    sem_init(&writeblock, 0, 1);

    pthread_attr_t attr[1];
    pthread_attr_init(&attr[0]);
    pthread_attr_setdetachstate(&attr[0], PTHREAD_CREATE_JOINABLE);

    // Use intptr_t instead of int in the loop variable
    for (intptr_t i = 0; i < numOfReaders; i++) {
        pthread_create(&readers[i], &attr[0], reader_thread, (void*) i);
    }
    for (intptr_t i = 0; i < numOfWriters; i++) {
        pthread_create(&writers[i], &attr[0], writer_thread, (void*) i);
    }

    pthread_join(writers[0], NULL);

    sem_destroy(&mutex);
    sem_destroy(&writeblock);

    return 0;
}
