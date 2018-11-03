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
    res->nodesNum = res->totalEntries = 1;

    return res;
}

void deleteResult(Result * res){
    Node* current;
    Node* temp;

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

void printResult(Result* res){
    Node* current = res->first;

    // std::cout << "totalEntries:" << res->totalEntries << '\n';
    // std::cout << "nodesNum:" << res->nodesNum << '\n';

    while(current != NULL){
        printNodeResult(current);
        current = current->next;
    }
}

void insertResult(Result * res, uint32_t rowidA, uint32_t rowidB){
    checkFullness(res);
    insertToNode(res->last,rowidA,rowidB);
    res->totalEntries ++;
}

/* If there is no space to insert new entry make a new node */
void checkFullness(Result * res){
    Node* temp;
    if(res->last->count == ENTRIES_PER_NODE){
        // We need a new node
        temp = newNode();
        res->last->next = temp;
        res->last = temp;
        res->nodesNum ++;
    }
}

uint32_t * getResultEntry(Result * res, uint32_t index){
    return NULL; //to be implemented
}


Node * newNode(){
    Node* temp;

    temp = new Node;
    CHECK_OR_EXIT(temp);

    temp->count = 0;
    temp->next = NULL;
    temp->buffer = new uint32_t[BUFFER_SIZE];
    CHECK_OR_EXIT(temp->buffer);

    memset(temp->buffer,0,BUFFER_SIZE);
    return temp;
}

void deleteNode(Node * node){
    delete[] node->buffer;
    delete node;
}

void insertToNode(Node * node, uint32_t rowidA, uint32_t rowidB){
    uint32_t offset = node->count * ENTRY_SIZE;
    memcpy(node->buffer + offset,&rowidA,sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(node->buffer + offset,&rowidB,sizeof(uint32_t));
    node->count ++;
}

void printNodeResult(Node * node){
    uint32_t rowidA,rowidB,offset=0;
    // std::cout << "count:" << node->count << '\n';
    for (uint32_t i = 0; i < node->count; i++) {
        memcpy(&rowidA,node->buffer + offset,sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&rowidB,node->buffer + offset,sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::cout << "rowidA:" << rowidA << " | rowidB:" << rowidB << '\n';
    }
}

uint32_t * getNodeEntry(Node * res, uint32_t index){
    return NULL; //to be implemented
}
