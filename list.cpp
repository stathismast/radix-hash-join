/***************************************************************************************
Header file : PostingList.h
Description : Implementation of methods of structs Bucket and Result,
              which are used to store and retrieve the results of a join query.
****************************************************************************************/

#include "list.hpp"

#define CHECK_OR_EXIT(value)     \
{                           \
	if (value == NULL) {      \
		std::cout << "Error at memory allocation! New() returned NULL! Program must exit!!" << std::endl;\
		exit(EXIT_FAILURE);           \
	}                         \
}

int entry_size = 0;
int entries_per_bucket = 0;

Result newResult(){
    Result temp;

    temp = new Header;
    CHECK_OR_EXIT(temp);

    temp->first = temp->last = NULL;
    temp->buckets_num = temp->total_entries = 0;

    return temp;
}

void destroyResult(Result this_result){
    BucketPtr current;
    BucketPtr temp;

    if(this_result == NULL) return;

    current = this_result->first;
    while(current != NULL){
        temp = current;
        current = current->next;
        destroyBucket(temp);
    }
    delete this_result;
}

bool isEmptyResult(Result this_result){
    return (this_result->first == NULL);
}

void printResult(Result this_result){
    BucketPtr temp = this_result->first;
    // std::cout << "total_entries_result:" << this_result->total_entries << '\n';
    // std::cout << "total_buckets_num:" << this_result->buckets_num << '\n';
    for (int i = 0; i < this_result->buckets_num; i++) {
        printBucketResult(temp);
        temp = getNextBucket(temp);
    }
}

BucketPtr getResultFirstBucket(Result this_result){
    return (this_result->first);
}

void insertResult(Result this_result,uint32_t rowidA,uint32_t rowidB){

    if(isEmptyResult(this_result)){ //it's first time
        entry_size = sizeof(rowidA) + sizeof(rowidB);
        entries_per_bucket = BUFFER_SIZE / entry_size;
        this_result->first = newBucket();
        this_result->last = this_result->first;
        this_result->buckets_num ++;
    }

    insertBucketEntry(this_result->last,rowidA,rowidB);
    this_result->total_entries ++;

    //prepare for next insertion
    prepareNextResult(this_result);

}

void prepareNextResult(Result this_result){
    BucketPtr temp;
    if(this_result->last->total_entries == entries_per_bucket){
        //we need a new bucket
        temp = newBucket();
        setNextBucket(this_result->last,temp);
        this_result->last = temp;
        this_result->buckets_num ++;
    }
}

uint32_t* getResultEntry(Result this_result,uint32_t index){
    return NULL; //to be implemented
}


BucketPtr newBucket(){
    BucketPtr temp;

    temp = new Bucket;
    CHECK_OR_EXIT(temp);

    temp->offset = 0;
    temp->total_entries = 0;
    temp->next = NULL;

    temp->buffer = new uint32_t[BUFFER_SIZE];
    CHECK_OR_EXIT(temp->buffer);

    memset(temp->buffer,0,BUFFER_SIZE);
    return temp;
}

void destroyBucket(BucketPtr this_bucket){
    delete[] this_bucket->buffer;
    delete this_bucket;
}

void setNextBucket(BucketPtr this_bucket,BucketPtr next_node){
    this_bucket->next = next_node;
}

BucketPtr getNextBucket(BucketPtr this_bucket){
    return this_bucket->next;
}

void insertBucketEntry(BucketPtr this_bucket,uint32_t rowidA,uint32_t rowidB){
    if(this_bucket->total_entries < entries_per_bucket){

        memcpy(this_bucket->buffer+this_bucket->offset,&rowidA,sizeof(uint32_t));
        this_bucket->offset += sizeof(uint32_t);
        memcpy(this_bucket->buffer+this_bucket->offset,&rowidB,sizeof(uint32_t));
        this_bucket->offset += sizeof(uint32_t);

        this_bucket->total_entries ++;
    }
}

void printBucketResult(BucketPtr this_bucket){
    uint32_t rowidA,rowidB,offset=0;
    // std::cout << "total_entries_bucket:" << this_bucket->total_entries << '\n';
    for (uint32_t i = 0; i < this_bucket->total_entries; i++) {
        memcpy(&rowidA,this_bucket->buffer + offset,sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&rowidB,this_bucket->buffer + offset,sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::cout << "rowidA:" << rowidA << " | rowidB:" << rowidB << '\n';
    }
}

uint32_t* getBucketEntry(Result* this_result,uint32_t index){
    return NULL; //to be implemented
}
