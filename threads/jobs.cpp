#include "jobs.hpp"
#include <iostream>

#include "../singleJoin/join.hpp"

#include "scheduler.hpp"

//global
uint64_t * histograms[4];
uint64_t * psums[4];
extern JobScheduler * myJobScheduler;

extern uint64_t * globalHistA;
extern uint64_t * globalPsumA;
extern uint64_t * globalHistB;
extern uint64_t * globalPsumB;

extern Result *** globalResults;

extern Column * orderedA;
extern Column * orderedB;

pthread_mutex_t memcpy_mtx;

// Takes A as input and returns A'
Column * bucketifyThread(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions){

                      
    pthread_mutex_init(&memcpy_mtx,NULL);

    // Calculate histograms
    uint64_t * startA = rel->value;

    //jobs for histogramA
    uint64_t length = rel->size / 4;
    uint64_t lastExtra = rel->size % 4;

    uint64_t i;

    for (i = 0; i < 4; i++) {
        histograms[i] = new uint64_t[numberOfBuckets];
        for(uint64_t j=0; j<numberOfBuckets; j++){
            histograms[i][j] = 0;
        }
    }

    for (i = 0; i < 3; i++) {
        myJobScheduler->Schedule(new HistogramJob(startA,length,&histograms[i]));
        startA += length;
    }
    //last thread may take extra length
    myJobScheduler->Schedule(new HistogramJob(startA,length+lastExtra,&histograms[i]));

    //std::cout << "Before barrier 1" << '\n';
    myJobScheduler->Barrier(4);
    //std::cout << "After barrier 1" << '\n';

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

    // Create the final ordered Column
    Column * threadOrdered = newColumn(rel->size);

    // Create partition jobs
    uint64_t start = 0;
    for (i = 0; i < 3; i++) {
        myJobScheduler->Schedule(new PartitionJob(rel,start,length,psums[i],
                                        numberOfBuckets,threadOrdered));
        start += length;
    }
    //last thread may take extra length
    myJobScheduler->Schedule(new PartitionJob(rel,start,length+lastExtra,psums[i],
                                    numberOfBuckets,threadOrdered));

    //std::cout << "Before barrier 2" << '\n';
    myJobScheduler->Barrier(4);
    //std::cout << "After barrier 2" << '\n';

    delete[] histograms[0];
    for (uint64_t i = 1; i < 4; i++) {
        delete[] histograms[i];
        delete[] psums[i];
    }

    // Calculate starting position of each bucket
    *startingPositions = psums[0];
    
    pthread_mutex_destroy(&memcpy_mtx);

    return threadOrdered;
}


void threadJoin(uint64_t numberOfBuckets){
    for (uint64_t i = 0; i < numberOfBuckets; i++){
        myJobScheduler->Schedule(new JoinJob(i));
    }

    myJobScheduler->Barrier((int) numberOfBuckets);
}

Result ** convertResult(uint64_t numberOfBuckets){
    Result ** result = new Result*[2];
    result[0] = newResult();
    result[1] = newResult();

    for (uint64_t i = 0; i < numberOfBuckets; i++){
        for(uint64_t j=0; j<globalResults[i][0]->totalEntries; j++){
            insertSingleResult(result[0], getSingleEntry(globalResults[i][0],j));
        }
        for(uint64_t j=0; j<globalResults[i][1]->totalEntries; j++){
            insertSingleResult(result[1], getSingleEntry(globalResults[i][1],j));
        }
    }
    return result;
}

void calculateThreadHistogram( uint64_t * start, uint64_t length, uint64_t * histogram ){

    // Add up the number of tuples in each bucket
    for(uint64_t i=0; i<length; i++)
        histogram[h1(start[i])]++;

    return;

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

    calculateThreadHistogram(start,length,*myHistogram);

    return 1;
}

PartitionJob::PartitionJob( Column * curOriginal,
                            uint64_t  curStart,uint64_t curLength,
                            uint64_t * curPsum, uint64_t curCount,
                            Column * curOrdered)
:original(curOriginal), start(curStart), length(curLength), myPsum(curPsum), bucketCount(curCount), ordered(curOrdered){
}

PartitionJob::~PartitionJob(){
    //std::cout << "A PartitionJob is destroyed!" << '\n';
}

uint64_t PartitionJob::Run(){

    uint64_t * offsets = new uint64_t[bucketCount];
    pthread_mutex_lock(&memcpy_mtx);
    memcpy(offsets, myPsum, bucketCount * sizeof(uint64_t));
    pthread_mutex_unlock(&memcpy_mtx);

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

JoinJob::JoinJob(uint64_t bucketNumber)
{
    this->bucketNumber = bucketNumber;
    //std::cout << "A JoinJob is created!" << '\n';
}

JoinJob::~JoinJob(){
}

uint64_t JoinJob::Run(){
    uint64_t * bucketArray;
    uint64_t * chainArray;

    Result *** result = &globalResults[bucketNumber];
    *result = new Result*[2];
    (*result)[0] = newResult();
    (*result)[1] = newResult();

    uint64_t i = bucketNumber;

    if(globalHistA[i] == 0 || globalHistB[i] == 0){
        //the one bucket is empty so there is nothing to compare with
        //the other bucket
        return 0;
    }
    // For each bucket find the smaller one and make an index with h2 for
    // that one. Then find the equal values and store them in result
    if (globalHistA[i] >= globalHistB[i]) {
        bucketify2(orderedB, globalHistB[i], globalPsumB[i], &bucketArray, &chainArray);
        compare(orderedA, orderedB, globalHistA[i], globalPsumA[i], globalHistB[i], \
            globalPsumB[i], bucketArray, chainArray, (*result), 0);
    }
    else {
        bucketify2(orderedA, globalHistA[i], globalPsumA[i], &bucketArray, &chainArray);
        compare(orderedB, orderedA, globalHistB[i], globalPsumB[i], globalHistA[i], \
            globalPsumA[i], bucketArray, chainArray, (*result), 1);
    }

    delete[] bucketArray;
    delete[] chainArray;

    return 1;
}
