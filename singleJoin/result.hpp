/***************************************************************************************
Header file : result.hpp
Description : Declaration of structs Node and Result which are used to
              represent results list which contains all the results of a join query.
****************************************************************************************/
#ifndef LIST_H
#define LIST_H

//#define BUFFER_SIZE 16
#define BUFFER_SIZE (1024*1024)

#define ENTRY_SIZE (sizeof(uint64_t) + sizeof(uint64_t))

#define ENTRIES_PER_NODE (BUFFER_SIZE / ENTRY_SIZE)

#include <stdint.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

typedef struct Node{
    uint64_t * buffer;
    Node * next;
    uint64_t count;
} Node;

typedef struct Result{
    Node * first;
    Node * last;
    uint64_t nodesNum;
    uint64_t totalEntries;
} Result;

#endif /* LIST_H */

Result * newResult();
void deleteResult(Result * res);
bool isEmptyResult(Result * res);
void printResult(Result * res);
void insertResult(Result * res,uint64_t rowidA,uint64_t rowidB);
uint64_t * getResultEntry(Result * res,uint64_t index);
void checkFullness(Result * res);

Node * newNode();
void deleteNode(Node * node);
void insertToNode(Node * node,uint64_t rowidA,uint64_t rowidB);
void printNodeResult(Node * node);
uint64_t * getNodeEntry(Node * node,uint64_t index);

void printSingleResult(Result * res);
void insertSingleResult(Result * res, uint64_t rowid);
void checkSingleFullness(Result * res);
void insertSingleToNode(Node * node, uint64_t rowid);
void printNodeSingleResult(Node * node);
