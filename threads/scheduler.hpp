#include <stdint.h>     // for uint64_t
#include "jobs.hpp"

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
char notEmpty(Queue * queue);
