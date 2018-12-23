#include "scheduler.hpp"

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
    free(node);
    return job;
}

char notEmpty(Queue * queue){
    return !(queue->first == NULL);
}
