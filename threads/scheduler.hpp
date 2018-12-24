#ifndef JS_H
#define JS_H

#include <stdint.h>     // for uint64_t
#include "jobs.hpp"
#include "threads.hpp"
#include <semaphore.h>

typedef struct Queue{
    struct QueueNode * first;
    struct QueueNode * last;
} Queue;

typedef struct QueueNode{
    Job * job;
    struct QueueNode * next;
} QueueNode;

Queue * newQueue();
QueueNode * newQueueNode(Job * job);
void addToQueue(Queue * queue, Job * job);
Job * popFromQueue(Queue * queue);
bool notEmpty(Queue * queue);
bool isEmpty(Queue * queue);

// Class JobScheduler
class JobScheduler {
    pthread_t * threadPool;
    uint64_t threadNum;
public:
    JobScheduler() = default;
    ~JobScheduler() = default;

    // Initializes the JobScheduler with the number of open threads.
    // Returns true if everything done right false else.
    bool Init(uint64_t num_of_threads);

    // Free all resources that the are allocated by JobScheduler
    // Returns true if everything done right false else.
    bool Destroy();

    // Waits Until executed all jobs in the queue.
    void Barrier();

    // Add a job in the queue
    // Returns true if everything done right false else.
    bool Schedule(Job* job);

    // Waits until all threads finish their job, and after that close all threads.
    void Stop();

};

#endif /* JS_H */
