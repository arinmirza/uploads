#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>

//there are only two "tokens" for five "slots"
#define BUFFER_SIZE 5

//declare function prototype of type "pointer to void"
void *thread_func(void *data);

//declare and initialize the number of threads
int n = 0;

//declare buffer array
int *buffers = NULL;
sem_t *semaphores = NULL;

int main(int argc, char **args) {
    //array of threads;
    pthread_t *threads = NULL;
    //array of thread id's (arguments for the thread)
    int *thread_id = NULL;
    //other variables
    int i;

    if (argc < 2) {
        printf("please provide the number of threads to create\n");
        return EXIT_FAILURE;
    }

    //convert first argument to integer (args[0] is the name of the program)
    n = atoi(args[1]);

    //range check (1-20), otherwise EXIT_FAILURE
    if (n > 20) {
        printf("number of threads is too large\n");
        return EXIT_FAILURE;
    } else if (n < 1) {
        printf("number of threads is too small\n");
        return EXIT_FAILURE;
    }

    //allocate memory for n threads
    threads = (pthread_t *) malloc(n * sizeof(pthread_t));
    //allocate memory for a buffer array of n integers
    buffers = (int *) malloc(n * sizeof(int));
    //allocate memory for thread arguments
    thread_id = (int *) malloc(n * sizeof(int));
    
    //allocate memory for the semaphores ***
    semaphores = (sem_t *) malloc(n * sizeof(sem_t));
    //initalize semaphores with 1
    for(i = 0; i < n; ++i) {
    	sem_init(&(semaphores[i]), 0, 1);
    }

    buffers[0] = 2;

    for (i = 1; i<n; i++) {
        buffers[i] = 0;
    }

    //create n individual threads and pass two parameters:
    //1. a pointer to the function the thread should call
    //2. the thread's id to be (properly typecasted)
    for (i = 0; i<n; i++) {
        thread_id[i] = i;
        //this will pass the addresses 0x001, 0x002 etc.
        if (pthread_create(&threads[i], NULL, thread_func, (void *)&thread_id[i]) ) {
            fprintf(stderr, "cannot create thread[%d]\n", i);
            exit(EXIT_FAILURE);
        }

    }

    //wait for each of the n threads
    for (i = 0; i<n; i++) {
        pthread_join(threads[i], NULL);
        printf("thread%d is done.\n", i);
    }

    //free allocated memory
    //we actually never reach this part because of the infinite loops
    free(threads);
    free(buffers);
    free(thread_id);

    return EXIT_SUCCESS;
}


//the function that is run in each thread
//access to any shared variable that is altered in this function must be synchronized!
void *thread_func(void *data) {
    int thread_id = *((int *)data);
    //mod n for circular handover
    int thread_successor_id = (thread_id+1)%n;

    printf("Thread #%d fired up\n", thread_id);

    //infinite loop (in C there's no "true" or "false", just 1 and 0)
    while(1) {
        //wait until there is something in buffer i to take
        while (buffers[thread_id] < 1)
            pthread_yield();
        //take one element
        sem_wait(&(semaphores[thread_id]));
        buffers[thread_id]--;
        printf("Thread #%d: element taken from buffer %d, %d elements in buffer\n",
                thread_id, thread_id, buffers[thread_id]);
        sem_post(&(semaphores[thread_id]))

        //wait until there's empty space in buffer (i+1) mod n
        while (buffers[thread_successor_id] >= BUFFER_SIZE)
            pthread_yield();
        sem_wait(&(semaphores[thread_successor_id]));
        //put one element into buffer
        buffers[thread_successor_id]++;
        printf("Thread #%d: element put into buffer %d, %d elements in buffer\n",
                thread_id, thread_successor_id, buffers[thread_successor_id]);
        sem_post(&(semaphores[thread_successor_id]));
        pthread_yield();
    }
}
