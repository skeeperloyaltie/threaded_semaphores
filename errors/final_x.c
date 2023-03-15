/* n reader 1 writer problem implementation using semaphores and threads */
/* By Skeeper Loyaltie */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#define MAX_READERS 14 /* Maximum number of readers */
#define NUM_ITERATIONS 250000000 /* Number of times to read the counter */

/* Global variables */
int counter = 0; /* Shared counter */
sem_t mutex; /* Semaphore to ensure mutual exclusion */
sem_t in_cs; /* Semaphore to signal the writer is in the critical section */

/* Reader thread function */
void* reader_thread(void* arg) {
    intptr_t reader_id = (intptr_t) arg; /* Reader ID passed as argument */
    int i;
    
    for (i = 0; i < NUM_ITERATIONS; i++) {
        /* Entry section */
        sem_wait(&mutex); /* Acquire mutex to access counter */
        if (sem_trywait(&in_cs) == 0) { /* Check if writer is in the critical section */
            printf("Error: reader %ld entered critical section while writer is in it!\n", reader_id);
            sem_post(&in_cs); /* Release in_cs */
        }
        sem_post(&mutex); /* Release mutex */
        
        /* Critical section */
        printf("Reader %ld reading counter: %d\n", reader_id, counter);
        
        /* Exit section */
        sem_wait(&mutex); /* Acquire mutex to access counter */
        sem_post(&mutex); /* Release mutex */
        
        /* Remainder section */
        relaxandspendtime(); /* Simulate work outside the critical section */
    }
    
    /* Print reader name when done */
    printf("Reader %ld is done.\n", reader_id);
    
    pthread_exit(NULL); /* Exit thread */
}

/* Writer thread function */
void* writer_thread(void* arg) {
    int i;
    
    for (i = 0; i < 25000; i++) { /* Write to the counter 25000 times */
        /* Entry section */
        sem_wait(&in_cs); /* Acquire in_cs to enter critical section */
        
        /* Critical section */
        counter++;
        
        /* Exit section */
        sem_post(&in_cs); /* Release in_cs to leave critical section */
        
        /* Remainder section */
        relaxandspendtime(); /* Simulate work outside the critical section */
    }
    
    /* Set shared flag in-cs to 0 before leaving critical section */
    sem_wait(&in_cs); /* Acquire in_cs to modify in-cs */
    sem_post(&in_cs); /* Release in_cs to modify in-cs */
    
    /* Print "Done" when writer is done */
    printf("Writer is done.\n");
    
    pthread_exit(NULL); /* Exit thread */
}
void relaxandspendtime() {
    /* Simulate some work */
    int i;
    for (i = 0; i < 100000000; i++) {
        /* Do nothing */
    }
}

/* Main function */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Please provide the number of readers as an argument.\n");
        return 1;
    }
    
    int numOfReaders = atoi(argv[1]);
    if (numOfReaders > 14) {
        printf("Number of readers should be at most 14.\n");
        return 1;
    }
    
    sem_init(&mutex, 0, 1);
    sem_init(&in_cs, 0, 0);
    
    pthread_t readers[numOfReaders], writer;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    sem_t roomEmpty; /* Semaphore to signal the room is empty */
    sem_t turnstile; /* Semaphore to coordinate access to the room */
    sem_init(&roomEmpty, 0, 1);
    sem_init(&turnstile, 0, 1);
    
    int k = numOfReaders / 2;
    for (intptr_t i = 0; i < k; i++) {
        pthread_create(&readers[i], &attr, reader_thread, (void*) i);
    }
    pthread_create(&writer, &attr, writer_thread, NULL);
    for (intptr_t i = k; i < numOfReaders; i++) {
        pthread_create(&readers[i], &attr, reader_thread, (void*) i);
    }
    
    for (intptr_t i = 0; i < numOfReaders; i++) {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer, NULL);
    
    sem_destroy(&mutex);
    sem_destroy(&in_cs);
    sem_destroy(&roomEmpty);
    sem_destroy(&turnstile);
    
    return 0;
}
