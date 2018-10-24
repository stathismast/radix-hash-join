/***************************************************************************************
Header file : PostingList.h
Description : Implementation of methods of classes BucketNode and ResultsList,
              which are used to store and retrieve the results of a join query.
****************************************************************************************/

#include <iostream>

using namespace std;

#include "ResultsList.hpp"

#include <cstdlib>

BucketNode::BucketNode()
:next(NULL),offset(0)
{
    my_results = new int32_t[BUCKET_SIZE];
}

BucketNode::~BucketNode(){
    delete[] my_results;
}

BucketNode* BucketNode::GetNext(){
    return next;
}

void BucketNode::SetNext(BucketNode* my_next){
    next = my_next;
}

int BucketNode::GetOffset(){
    return offset;
}

ResultsList::ResultsList()
:first(NULL),last(NULL),num_of_buckets(0)
{

}

ResultsList::~ResultsList(){
    BucketNode* current;

    while( first != NULL ){
        current = first;
        first = current->GetNext();
        delete current;
    }

}

void ResultsList::Insert( Result& result_to_insert ){

}

int ResultsList::GetBucketsNum(){
    return num_of_buckets;
}

bool ResultsList::IsEmpty(){
    return ( first == NULL );
}

void ResultsList::PrintResults(){

}

BucketNode* ResultsList::GetFirst(){
    return first;
}
