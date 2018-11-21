/***************************************************************************************
Header file : result.cpp
Description : Implementation of methods of structs Node and Result,
              which are used to store and retrieve the results of a join query.
****************************************************************************************/

#include "result.hpp"

#define CHECK_OR_EXIT(value)                                        \
{                                                                   \
    if (value == NULL) {                                            \
        std::cout << "Error at memory allocation." << std::endl;    \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
}

Result * newResult(){
    Result * res;

    res = new Result;
    CHECK_OR_EXIT(res);

    res->first = newNode();
    res->last = res->first;
    res->totalEntries = 0;
    res->nodesNum = 1;

    return res;
}

void deleteResult(Result * res){
    Node * current;
    Node * temp;

    if(res == NULL) return;

    current = res->first;
    while(current != NULL){
        temp = current;
        current = current->next;
        deleteNode(temp);
    }
    delete res;
}

bool isEmptyResult(Result * res){
    return (res->first == NULL);
}

void printDoubleResult(Result * res){
    Node * current = res->first;
    while(current != NULL){
        printNodeDoubleResult(current);
        current = current->next;
    }
}

void printSingleResult(Result * res){
    Node * current = res->first;
    while(current != NULL){
        printNodeSingleResult(current);
        current = current->next;
    }
}

void insertDoubleResult(Result * res, uint64_t rowidA, uint64_t rowidB){
    checkDoubleFullness(res);
    insertDoubleToNode(res->last,rowidA,rowidB);
    res->totalEntries ++;
}

void insertSingleResult(Result * res, uint64_t rowid){
    checkSingleFullness(res);
    insertSingleToNode(res->last,rowid);
    res->totalEntries ++;
}

/* If there is no space to insert new entry make a new node */
void checkDoubleFullness(Result * res){
    Node * temp;
    if(res->last->count == ENTRIES_PER_NODE){
        // We need a new node
        temp = newNode();
        res->last->next = temp;
        res->last = temp;
        res->nodesNum ++;
    }
}

void checkSingleFullness(Result * res){
    Node * temp;
    if(res->last->count == BUFFER_SIZE / sizeof(uint64_t)){
        // We need a new node
        temp = newNode();
        res->last->next = temp;
        res->last = temp;
        res->nodesNum ++;
    }
}

uint64_t * getResultEntry(Result * res, uint64_t index){
    return NULL; //to be implemented
}


Node * newNode(){
    Node * temp;

    temp = new Node;
    CHECK_OR_EXIT(temp);

    temp->count = 0;
    temp->next = NULL;
    temp->buffer = (uint64_t *) new char[BUFFER_SIZE]; //1024*1024 bytes
    CHECK_OR_EXIT(temp->buffer);

    memset(temp->buffer,0,BUFFER_SIZE);
    return temp;
}

void deleteNode(Node * node){
    delete[] node->buffer;
    delete node;
}

void insertDoubleToNode(Node * node, uint64_t rowidA, uint64_t rowidB){
    uint64_t offset = node->count * 2;
    memcpy(node->buffer + offset,&rowidA,sizeof(uint64_t));
    offset++;
    memcpy(node->buffer + offset,&rowidB,sizeof(uint64_t));
    node->count ++;
}

void insertSingleToNode(Node * node, uint64_t rowid){
    uint64_t offset = node->count;
    memcpy(node->buffer + offset,&rowid,sizeof(uint64_t));
    node->count ++;
}

void printNodeDoubleResult(Node * node){
    uint64_t rowidA,rowidB,offset=0;
    // std::cout << "count:" << node->count << '\n';
    for (uint64_t i = 0; i < node->count; i++) {
        memcpy(&rowidA,node->buffer + offset,sizeof(uint64_t));
        offset++;
        memcpy(&rowidB,node->buffer + offset,sizeof(uint64_t));
        offset++;
        std::cout << "rowidA:" << rowidA << " | rowidB:" << rowidB << '\n';
    }
}

void printNodeSingleResult(Node * node){
    uint64_t rowid,offset=0;
    // std::cout << "count:" << node->count << '\n';
    for (uint64_t i = 0; i < node->count; i++) {
        memcpy(&rowid,node->buffer + offset,sizeof(uint64_t));
        offset++;
        std::cout << "rowid:" << rowid << '\n';
    }
}

uint64_t * getNodeEntry(Node * res, uint64_t index){
    return NULL; //to be implemented
}

void printResult(Result * res, uint64_t entryCount){
    Node * current = res->first;
    while(current != NULL){
        printNodeResult(current, entryCount);
        current = current->next;
    }
}

void insertResult(Result * res, uint64_t * entries, uint64_t entryCount){
    checkFullness(res, entryCount);
    insertToNode(res->last, entries, entryCount);
    res->totalEntries ++;
}

void checkFullness(Result * res, uint64_t entryCount){
    Node * temp;
    if(res->last->count == BUFFER_SIZE / (entryCount * sizeof(uint64_t))){
        // We need a new node
        temp = newNode();
        res->last->next = temp;
        res->last = temp;
        res->nodesNum ++;
    }
}

void insertToNode(Node * node, uint64_t * entries, uint64_t entryCount){
    uint64_t offset = node->count * entryCount;
    memcpy(node->buffer + offset, entries, entryCount * sizeof(uint64_t));
    node->count ++;
}

void printNodeResult(Node * node, uint64_t entryCount){
    uint64_t entry;
    // std::cout << "count:" << node->count << '\n';
    for (uint64_t i=0; i<node->count; i++) {
        for(uint64_t j=0; j<entryCount; j++){
            memcpy(&entry,node->buffer + entryCount*i + j, sizeof(uint64_t));
            std::cout << entry << '|';
        }
        std::cout << std::endl;
    }
}
