#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h> // Add this line to include intptr_t type

#define MAX_READERS 40

int shared_resource = 0;
sem_t mutex, writeblock;
int numOfReaders, numOfWriters;
void relaxandspendtime()
{
    int i;
    for (i = 0; i < 250000000; i++)
        i = i;
}

void *reader_thread(void *arg)
{
    intptr_t thread_id = (intptr_t)arg;
    int count = 0;
    while (count < 25000)
    {
        sem_wait(&mutex);
        shared_resource++;
        if (shared_resource == 1)
        {
            sem_wait(&writeblock);
        }
        sem_post(&mutex);

        printf("Reader %ld: Done %d, last read value = %d\n", thread_id, count, shared_resource);
        count++;

        relaxandspendtime(); // Add this line

        sem_wait(&mutex);
        shared_resource--;
        if (shared_resource == 0)
        {
            sem_post(&writeblock);
        }
        sem_post(&mutex);
    }
}

void *writer_thread(void *arg)
{
    intptr_t thread_id = (intptr_t)arg;
    int count = 0;
    while (count < 25000)
    {
        sem_wait(&writeblock);
        shared_resource++;
        printf("Writer %ld: Done %d, last read value %d\n", thread_id, count, shared_resource);
        count++;
        sem_post(&writeblock);

        relaxandspendtime(); // Add this line
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <number of readers> <number of writers>\n", argv[0]);
        return 1;
    }

    numOfReaders = atoi(argv[1]);
    if (numOfReaders > MAX_READERS)
    {
        printf("Error: too many readers (max %d)\n", MAX_READERS);
        return 1;
    }

    numOfWriters = atoi(argv[2]);

    pthread_t readers[numOfReaders], writers[numOfWriters];

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&writeblock, 0, 1);

    pthread_attr_t attr[1];
    pthread_attr_init(&attr[0]);
    pthread_attr_setdetachstate(&attr[0], PTHREAD_CREATE_JOINABLE);

    // Create reader threads
    int k = (int)(numOfReaders / 2);
    for (intptr_t i = 0; i < k; i++)
    {
        pthread_create(&readers[i], &attr[0], reader_thread, (void *)i);
    }

    // Create writer thread
    pthread_create(&writers[0], &attr[0], writer_thread, NULL);

    for (intptr_t i = k; i < numOfReaders; i++)
    {
        pthread_create(&readers[i], &attr[0], reader_thread, (void *)i);
    }

    // Wait for writer thread to finish
    pthread_join(writers[0], NULL);

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&writeblock);

    return 0;
}