/***************************************************************************************
Header file : ResultsList.hpp
Description : Declaration of structs Node,Result which are used to
              represent results list which contains all the results of a join query.
****************************************************************************************/
#ifndef LIST_H
#define LIST_H

// #define BUFFER_SIZE 20
#define BUFFER_SIZE 1024*1024

#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

typedef struct Header* Result;
typedef struct Bucket* BucketPtr;

typedef struct Bucket{
    uint32_t* buffer;
    BucketPtr next;
    uint32_t total_entries;
    uint32_t offset;
} Bucket;

typedef struct Header{
    BucketPtr first;
    BucketPtr last;
    uint32_t buckets_num;
    uint32_t total_entries;
} Header;

Result newResult();
void destroyResult(Result this_result);
bool isEmptyResult(Result this_result);
void printResult(Result this_result);
BucketPtr getResultFirstBucket(Result this_result);
void insertResult(Result this_result,uint32_t rowidA,uint32_t rowidB);
uint32_t* getResultEntry(Result this_result,uint32_t index);
void prepareNextResult(Result this_result);

BucketPtr newBucket();
void destroyBucket(BucketPtr this_bucket);
void setNextBucket(BucketPtr this_bucket,BucketPtr next_node);
BucketPtr getNextBucket(BucketPtr this_bucket);
void insertBucketEntry(BucketPtr this_bucket,uint32_t rowidA,uint32_t rowidB);
void printBucketResult(BucketPtr this_bucket);
uint32_t* getBucketEntry(BucketPtr this_bucket,uint32_t index);

#endif /* LIST_H */
