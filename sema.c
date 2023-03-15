
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_READERS 14

// Global variables
int numOfReaders;
int sharedCounter;
int in_cs;
sem_t mutex;
sem_t wrt;

// Function prototypes
void *reader_thread(void *arg);
void *writer_thread(void *arg);

int main(int argc, char *argv[]) {
    // Check for valid number of readers
    if (argc != 2) {
        printf("Error: Invalid number of arguments.\n");
        exit(1);
    }
    numOfReaders = atoi(argv[1]);
    if (numOfReaders > MAX_READERS || numOfReaders < 1) {
        printf("Error: Invalid number of readers.\n");
        exit(1);
    }

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    // Create reader threads
    pthread_t readers[numOfReaders];
    pthread_attr_t attr[1];
    pthread_attr_init(&attr[0]);
    int k = (int) (numOfReaders/2);

    for (intptr_t i = 0; i < k; i++) {
    pthread_create(&readers[i], &attr[0], reader_thread, (void*) i);
    }

    // Create writer thread
    pthread_t writer;
    pthread_create(&writer, &attr[0], writer_thread, NULL);
    for (intptr_t i = k; i < numOfReaders; i++) {
        pthread_create(&readers[i], &attr[0], reader_thread, (void*) i);
    }

    // Join threads
    for (int i = 0; i < numOfReaders; i++) {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}

void *reader_thread(void *arg) {
    intptr_t thread_id = (intptr_t) arg;
    for (int i = 0; i < 250000000; i++) {
        // Entry section
        sem_wait(&mutex);
        if (in_cs == 1) {
            printf("Error: Reader %d attempted to enter critical section while writer was in it.\n", (int) thread_id);
            sem_post(&mutex);
            return NULL;
        }
        in_cs++;
        sem_post(&mutex);

        // Critical section
        int temp = sharedCounter;

        // Exit section
        sem_wait(&mutex);
        in_cs--;
        sem_post(&mutex);
    }
    printf("Reader %d finished.\n", (int) thread_id);
    return NULL;
}


void *writer_thread(void *arg) {
    for (int i = 0; i < 25000; i++) {
        // Entry section
        sem_wait(&wrt);
        in_cs = 1;

        // Critical section
        sharedCounter++;

        // Exit section
        in_cs = 0;
        sem_post(&wrt);
    }
    printf("Writer finished.\n");
    return NULL;
}