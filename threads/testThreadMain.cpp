#include "scheduler.hpp"
#include <iostream>

extern Queue * globalQueue;

extern pthread_mutex_t mutex;
extern sem_t count;

int main(int argc, char const *argv[]) {

    //initialize mutex and semaphore
    sem_init(&count,0,0);
    pthread_mutex_init(&mutex,NULL);

    globalQueue = newQueue();

    JobScheduler * myJobScheduler = new JobScheduler();

    myJobScheduler->Init(4);

    for (uint64_t i = 0; i < 15; i++) {
        myJobScheduler->Schedule(new JoinJob(i));
    }

    myJobScheduler->Stop();

    myJobScheduler->Destroy();

    delete myJobScheduler;
    delete globalQueue;

    //destroy mutex and semaphore
    sem_destroy(&count);
    pthread_mutex_destroy(&mutex);

    return 0;
}
