#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdint.h>

pthread_t createThread(void *(*start_routine) (void *));
void joinThread(pthread_t thread);
int terminateThread(pthread_t thread);
pthread_t * createThreadPool(void *(*start_routine) (void *), uint64_t size);

#endif /* THREADS_H */
