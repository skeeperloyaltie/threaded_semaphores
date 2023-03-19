#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h> // Add this line to include intptr_t type

#define MAX_READERS 14

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
    intptr_t thread_id = (intptr_t)arg; // Replace int with intptr_t
    for (int i = 0; i < 250000000; i++)
    {
        sem_wait(&mutex);
        if (shared_resource == -1)
        {
            printf("Error: Writer is in critical section!\n");
        }
        shared_resource++;
        sem_post(&mutex);
        relaxandspendtime();
    }
    sem_wait(&mutex);
    shared_resource--;
    sem_post(&mutex);
    printf("Reader %ld Done 25M, last read value = %d\n", thread_id, shared_resource);
    pthread_exit(NULL);
}

void *writer_thread(void *arg)
{
    for (int i = 0; i < 25000; i++)
    {
        sem_wait(&writeblock);
        shared_resource = i;
        sem_post(&writeblock);
        relaxandspendtime();
    }
    sem_wait(&writeblock);
    shared_resource = -1;
    sem_post(&writeblock);
    printf("Writer Done 25k, last value read = %d\n", shared_resource);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <number of readers>\n", argv[0]);
        return 1;
    }

    numOfReaders = atoi(argv[1]);
    if (numOfReaders > MAX_READERS)
    {
        printf("Error: too many readers (max %d)\n", MAX_READERS);
        return 1;
    }
    else if (numOfReaders < 1)
    {
        printf("Error: at least one reader is required\n");
        return 1;
    }

    pthread_t readers[numOfReaders], writer;
    intptr_t i;

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&writeblock, 0, 1);

    pthread_attr_t attr[1];
    pthread_attr_init(&attr[0]);
    pthread_attr_setdetachstate(&attr[0], PTHREAD_CREATE_JOINABLE);

    // Create reader threads
    for (i = 0; i < numOfReaders; i++)
    {
        pthread_create(&readers[i], &attr[0], reader_thread, (void *)i);
    }

    // Create writer thread
    pthread_create(&writer, &attr[0], writer_thread, NULL);

    // Wait for threads to finish
    for (i = 0; i < numOfReaders; i++)
    {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);

    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&writeblock);

    return 0;
}
