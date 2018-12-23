#include "scheduler.hpp"

pthread_mutex_t mutex;
sem_t count;
bool threadFinish;

Queue * globalQueue;

Queue * newQueue(){
    Queue * queue = new Queue;
    *queue = (Queue){0};
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

QueueNode * newNode(Job * job){
    QueueNode * node = new QueueNode;
    node->job = job;
    node->next = NULL;
    return node;
}

void addToQueue(Queue * queue, Job * job){
    if(queue->first == NULL){
        queue->first = newNode(job);
        queue->last = queue->first;
    }
    else{
        queue->last->next = newNode(job);
        queue->last = queue->last->next;
	}
}

Job * popFromQueue(Queue * queue){
    QueueNode * node = queue->first;
    queue->first = queue->first->next;

    Job * job = node->job;
    delete node;
    return job;
}

char notEmpty(Queue * queue){
    return !(queue->first == NULL);
}

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

void * myRoutine(void *arg){

    sem_wait(&count); //we use sem_wait at the end of loop so as we can check
                      //if we should terminate

    while(!threadFinish){

        pthread_mutex_lock(&mutex);
        Job * curJob = popFromQueue(globalQueue);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&printMutex);
        std::cout << "Hello from thread " << pthread_self() << '\n';
        curJob->Run();
        pthread_mutex_unlock(&printMutex);

        delete curJob;

        sem_wait(&count);
    }

    return NULL;
}

bool JobScheduler::Init(uint64_t num_of_threads){
    threadNum = num_of_threads;
    threadFinish = false;
    threadPool = createThreadPool(&myRoutine,threadNum);
    return true;
}

bool JobScheduler::Destroy(){
    delete[] threadPool;
}

void JobScheduler::Barrier(){
    return;
}

bool JobScheduler::Schedule(Job* job){

    pthread_mutex_lock(&mutex);
    addToQueue(globalQueue,job);
    pthread_mutex_unlock(&mutex);
    sem_post(&count);

    return true;
}

void JobScheduler::Stop(){

    //VVV to be replaced VVV
    uint64_t counter = 1;
    sem_getvalue (&count,(int*) &counter);

    while( counter > 0 ){
        sem_getvalue (&count,(int*) &counter);
    }
    //^^^ to be replaced ^^^


    //send "signal" to all threads to exit
    threadFinish = true;
    //some threads may be stuck on count semaphore so do sem_post(&count);
    for( uint64_t i = 0; i < threadNum; i ++ ){
        sem_post(&count);
    }

    for( uint64_t i = 0; i < threadNum; i ++ ){
        joinThread(threadPool[i]);
    }

}
