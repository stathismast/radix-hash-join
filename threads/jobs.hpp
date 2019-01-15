#ifndef JOBS_H
#define JOBS_H

#include <iostream>
#include "../singleJoin/structs.hpp"
#include "../singleJoin/h1.hpp"
#include "../singleJoin/result.hpp"

extern uint64_t numberOfBuckets;

void calculateThreadHistogram(uint64_t * start, uint64_t length, uint64_t * histogram);
Column * bucketifyThread(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions);

void threadJoin(uint64_t);
Result ** convertResult(uint64_t numberOfBuckets);

// Abstract Class Job
class Job {

public:
    Job() = default;
    virtual ~Job() {
        //  std::cerr << "A job will be destroyed!" << '\n';
      }

    // This method should be implemented by subclasses.
    virtual uint64_t Run() = 0;
};

class HistogramJob : public Job{
    uint64_t * start;
    uint64_t length;
    uint64_t ** myHistogram;
public:
    HistogramJob( uint64_t * curStart, uint64_t curLength,
                  uint64_t ** curGlobalPos);
    ~HistogramJob();
    uint64_t Run();
};

class PartitionJob : public Job{
    Column * original;
    uint64_t start;
    uint64_t length;
    uint64_t * myPsum;
    uint64_t bucketCount;
    Column * ordered;

public:
    PartitionJob(Column * curOriginal, uint64_t curStart, uint64_t curLength,
                 uint64_t * myPsum, uint64_t curCount, Column * curOrdered);
    ~PartitionJob();
    uint64_t Run();
};

class JoinJob : public Job{
    uint64_t bucketNumber;
public:
    JoinJob( uint64_t x );
    ~JoinJob();
    uint64_t Run();
};

#endif /* JOBS_H */
