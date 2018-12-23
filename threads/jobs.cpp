#include "jobs.hpp"
#include <iostream>

//global
uint64_t * histograms[4];
uint64_t * psums[4];

// Takes A as input and returns A'
Column * bucketifyThread(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions){

    // Calculate histograms
    Job * jobsArray[4];
    uint64_t * startA = rel->value;

    //jobs for histogramA
    uint64_t length = rel->size / 4;
    uint64_t lastExtra = rel->size % 4;

    uint64_t i;
    for (i = 0; i < 3; i++) {
        jobsArray[i] = new HistogramJob(startA,length,&histograms[i]);
        startA += length;
    }
    //last thread may take extra length
    jobsArray[i] = new HistogramJob(startA,length+lastExtra,&histograms[i]);

    for (uint64_t i = 0; i < 4; i++) {
        jobsArray[i]->Run();
    }

    //construct the whole histogram
    uint64_t * wholeHistogram = new uint64_t[numberOfBuckets];
    for(uint64_t i=0; i<numberOfBuckets; i++){
        wholeHistogram[i] = 0;
        for (int j = 0; j < 4; j++) {
            wholeHistogram[i] += histograms[j][i];
        }
    }

    *histogram = wholeHistogram;

    //calculate psums for each next thread
    for (uint64_t i = 0; i < 4; i++) {
        psums[i] = new uint64_t[numberOfBuckets];
    }

    //for first psum
    psums[0][0] = 0;
    for (uint64_t i = 1; i < 4; i++) {
        psums[i][0] = psums[i-1][0] + histograms[i-1][0];
    }

    //for rest psums
    for(uint64_t i=1; i<numberOfBuckets; i++){
        psums[0][i] = psums[3][i-1] + histograms[3][i-1];
        for(uint64_t j = 1; j < 4; j++){
            psums[j][i] = psums[j-1][i] + histograms[j-1][i];
        }
    }

    //delete HistogramJobs
    for (uint64_t i = 0; i < 4; i++) {
        delete jobsArray[i];
    }

    // Create the final ordered Column
    Column * threadOrdered = newColumn(rel->size);

    // Create partition jobs
    uint64_t start = 0;
    for (i = 0; i < 3; i++) {
        jobsArray[i] = new PartitionJob(rel,start,length,psums[i],
                                        numberOfBuckets,threadOrdered);
        start += length;
    }
    //last thread may take extra length
    jobsArray[i] = new PartitionJob(rel,start,length+lastExtra,psums[i],
                                    numberOfBuckets,threadOrdered);

    for (uint64_t i = 0; i < 4; i++) {
        jobsArray[i]->Run();
    }

    for (uint64_t i = 0; i < 4; i++) {
        delete jobsArray[i];
        delete[] histograms[i];
        delete[] psums[i];
    }

    // Calculate starting position of each bucket
    *startingPositions = calculateStartingPositions(*histogram);

    return threadOrdered;
}

uint64_t * calculateThreadHistogram( uint64_t * start, uint64_t length ){

    // Create and initialize histogram
    uint64_t * histogram = new uint64_t[numberOfBuckets];
    for(uint64_t i=0; i<numberOfBuckets; i++)
        histogram[i] = 0;

    // Add up the number of tuples in each bucket
    for(uint64_t i=0; i<length; i++)
        histogram[h1(start[i])]++;
    return histogram;

}

HistogramJob::HistogramJob( uint64_t * curStart, uint64_t curLength,
                            uint64_t ** curGlobalPos)
:start(curStart),length(curLength),myHistogram(curGlobalPos)
{
    //std::cout << "A HistogramJob is created!" << '\n';
}

HistogramJob::~HistogramJob(){
    //std::cout << "A HistogramJob is destroyed!" << '\n';
}

uint64_t HistogramJob::Run(){
    //std::cout << "A HistogramJob is running!" << '\n';

    *myHistogram = calculateThreadHistogram(start,length);

    return 1;
}

PartitionJob::PartitionJob( Column * curOriginal,
                            uint64_t  curStart,uint64_t curLength,
                            uint64_t * curPsum, uint64_t curCount,
                            Column * curOrdered)
:original(curOriginal), start(curStart), length(curLength), myPsum(curPsum), bucketCount(curCount), ordered(curOrdered){
}

PartitionJob::~PartitionJob(){
}

uint64_t PartitionJob::Run(){

    uint64_t * offsets = new uint64_t[bucketCount];
    memcpy(offsets, myPsum, bucketCount * sizeof(uint64_t));

    for(uint64_t i=start; i<start+length; i++){
        uint64_t val = original->value[i];

        // Store value & rowid of Tuple in the appropriate position
        ordered->value[offsets[h1(val)]] = val;
        ordered->rowid[offsets[h1(val)]] = original->rowid[i];

        // Increment starting position of the bucket since we just added to it
        offsets[h1(val)]++;
    }

    delete[] offsets;

    return 1;
}

JoinJob::JoinJob(uint64_t x)
{
    this->x = x;
    //std::cout << "A JoinJob is created!" << '\n';
}

JoinJob::~JoinJob(){
    std::cout << "A JoinJob is destroyed!" << '\n';
}

uint64_t JoinJob::Run(){
    std::cout << "A JoinJob with x - " << x << " - is running!" << '\n';
    return 1;
}
