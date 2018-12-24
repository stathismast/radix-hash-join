#include "threads.hpp"

#define perrorThreads(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))

// Create and return a thread
pthread_t createThread(void *(*start_routine) (void *)){
    pthread_t thread;

    pthread_create(&thread, NULL, start_routine, NULL);
    // printf("%ld: Created thread %ld\n", pthread_self(), thread);
    return thread;
}

void joinThread(pthread_t thread){
    // int err, status;

    pthread_join(thread, NULL);

    //  pthread_join(thread, (void **) &status);
    // printf("Thread %ld exited with code %d\n", thread, status);
}

int terminateThread(pthread_t thread){
    return pthread_cancel(thread);
}

pthread_t * createThreadPool(void *(*start_routine) (void *), uint64_t size){
    pthread_t * pool = new pthread_t[size];
    for(uint64_t i=0; i<size; i++){
        pool[i] = createThread(start_routine);
    }
    return pool;
}
