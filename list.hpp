/***************************************************************************************
Header file : list.hpp
Description : Declaration of structs Node and Result which are used to
              represent results list which contains all the results of a join query.
****************************************************************************************/
#ifndef LIST_H
#define LIST_H

//#define BUFFER_SIZE 20
#define BUFFER_SIZE (1024*1024)

#define ENTRY_SIZE (sizeof(uint32_t) + sizeof(uint32_t))

#define ENTRIES_PER_NODE (BUFFER_SIZE / ENTRY_SIZE)

#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

typedef struct Node{
    uint32_t* buffer;
    Node* next;
    uint32_t count;
} Node;

typedef struct Result{
    Node* first;
    Node* last;
    uint32_t nodesNum;
    uint32_t totalEntries;
} Result;

Result* newResult();
void destroyResult(Result* res);
bool isEmptyResult(Result* res);
void printResult(Result* res);
void insertResult(Result* res,uint32_t rowidA,uint32_t rowidB);
uint32_t* getResultEntry(Result* res,uint32_t index);
void prepareInsertion(Result* res);

Node* newNode();
void destroyNode(Node* node);
void insertNodeEntry(Node* node,uint32_t rowidA,uint32_t rowidB);
void printNodeResult(Node* node);
uint32_t* getNodeEntry(Node* node,uint32_t index);

#endif /* LIST_H */
