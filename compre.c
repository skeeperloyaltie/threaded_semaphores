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
void relaxandspendtime();

int main(int argc, char *argv[])
{
    // Check for valid number of readers
    if (argc != 2)
    {
        printf("Usage: ./nreader1writer <numOfReaders>\n");
        exit(1);
    }
    numOfReaders = atoi(argv[1]);
    if (numOfReaders > MAX_READERS || numOfReaders < 1)
    {
        printf("Number of readers must be between 1 and %d\n", MAX_READERS);
        exit(1);
    }

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    // Create reader and writer threads
    pthread_t readers[numOfReaders];
    pthread_t writer;
    pthread_attr_t attr[2];
    pthread_attr_init(&attr[0]);
    pthread_attr_init(&attr[1]);

    int k = (int) (numOfReaders/2);
    for(int i = 0; i < k; i++)
    {
        pthread_create(&readers[i], &attr[0], reader_thread, (void*) i);
    }
    /* Create the writer thread */
    pthread_create(&writer, &attr[1], writer_thread, NULL);
    for(int i = k ; i < numOfReaders ; i++)
    {
        pthread_create(&readers[i], &attr[0], reader_thread, (void*) i);
    }

    // Join threads
    for(int i = 0; i < numOfReaders; i++)
    {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);
    printf("Writer finished\n");
    // Destroy semaphores
    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}

void *reader_thread(void *arg)
{
    int thread_id = (int) arg;
    int last_read_value;

    // Entry section
    sem_wait(&mutex);
    if (in_cs == 1)
    {
        printf("Error: Reader %d attempted to enter critical section while writer was in it.\n", thread_id);
        sem_post(&mutex);
        return NULL;
    }
    sem_post(&mutex);

    // Critical section
    for (int i = 0; i < 250000000; i++)
    {
        last_read_value = sharedCounter;
        sharedCounter++;
    }
    printf("Reader %d finished\n", thread_id);
    

    // Exit section
    sem_post(&wrt);

    return NULL;
}

void *writer_thread(void *arg)
{
    int last_read_value;

    // Entry section
    sem_wait(&wrt);

    // Critical section
    in_cs = 1;
    for (int i = 0; i < 25000; i++)
    {
        last_read_value = sharedCounter;
        sharedCounter++;
        
    }
    

    // Exit section
    in_cs = 0;
    sem_post(&wrt);

    return NULL;
}

void relaxandspendtime()
{
    int i;
    for(i = 0; i < 250000000; i++) i=i;
}