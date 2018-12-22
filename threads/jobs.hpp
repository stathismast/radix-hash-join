#ifndef JOBS_H
#define JOBS_H

#include <iostream>
#include "../singleJoin/structs.hpp"
#include "../singleJoin/h1.hpp"

extern uint64_t numberOfBuckets;

uint64_t * calculateThreadHistogram( uint64_t * start, uint64_t length );
Column * bucketifyThread(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions);

// Abstract Class Job
class Job {

public:
    Job() = default;
    virtual ~Job() {
        //  std::cout << "A job will be destroyed!" << '\n';
      }

    // This method should be implemented by subclasses.
    virtual uint64_t Run() = 0;
};

class HistogramJob : public Job{
    uint64_t *start;
    uint64_t length;
public:
    uint64_t * myHistogram;
    HistogramJob( uint64_t * curStart, uint64_t curLength);
    ~HistogramJob();
    uint64_t Run();
};

class PartitionJob : public Job{

public:
    PartitionJob();
    ~PartitionJob();
    uint64_t Run();
};

class JoinJob : public Job{

public:
    JoinJob();
    ~JoinJob();
    uint64_t Run();
};

// // Class JobScheduler
// class JobScheduler {
//
// public:
//     JobScheduler() = default;
//     ~JobScheduler() = default;
//
//     // Initializes the JobScheduler with the number of open threads.
//     // Returns true if everything done right false else.
//     bool Init(uint64_t num_of_threads);
//
//     // Free all resources that the are allocated by JobScheduler
//     // Returns true if everything done right false else.
//     bool Destroy();
//
//     // Waits Until executed all jobs in the queue.
//     void Barrier();
//
//     // Add a job in the queue and returns a JobId
//     JobID Schedule(Job* job);
//
//     // Waits until all threads finish their job, and after that close all threads.
//     void Stop();
//
// };

#endif /* JOBS_H */
