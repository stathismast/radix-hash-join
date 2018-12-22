#include "jobs.hpp"
#include <iostream>

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

HistogramJob::HistogramJob( uint64_t * curStart, uint64_t curLength)
:start(curStart),length(curLength)
{
    //std::cout << "A HistogramJob is created!" << '\n';
}

HistogramJob::~HistogramJob(){
    //std::cout << "A HistogramJob is destroyed!" << '\n';
    delete[] myHistogram;
}

uint64_t HistogramJob::Run(){
    //std::cout << "A HistogramJob is running!" << '\n';

    myHistogram = calculateThreadHistogram(start,length);

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
