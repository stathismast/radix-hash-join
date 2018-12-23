#include <pthread.h>

pthread_t createThread(void *(*start_routine) (void *));
void joinThread(pthread_t thread);
int terminateThread(pthread_t thread);
