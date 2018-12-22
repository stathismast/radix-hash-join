#include "jobs.hpp"
#include <iostream>

//global
uint64_t * histograms[4];

// Takes A as input and returns A'
Column * bucketifyThread(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions){

    // Calculate histograms
    HistogramJob* jobsArray[4];
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

    for (uint64_t i = 0; i < 4; i++) {
        delete jobsArray[i];
        delete[] histograms[i];
    }

    // Calculate starting position of each bucket
    *startingPositions = calculateStartingPositions(*histogram);

    // Order the given touples bucket by bucket (basically produces A' from A)
    Column * ordered = order(rel, *startingPositions);

    return ordered;
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

PartitionJob::PartitionJob(){
    std::cout << "A PartitionJob is created!" << '\n';
}

PartitionJob::~PartitionJob(){
    std::cout << "A PartitionJob is destroyed!" << '\n';
}

uint64_t PartitionJob::Run(){
    std::cout << "A PartitionJob is running!" << '\n';
    return 1;
}

JoinJob::JoinJob(){
    std::cout << "A JoinJob is created!" << '\n';
}

JoinJob::~JoinJob(){
    std::cout << "A JoinJob is destroyed!" << '\n';
}

uint64_t JoinJob::Run(){
    std::cout << "A JoinJob is running!" << '\n';
    return 1;
}
