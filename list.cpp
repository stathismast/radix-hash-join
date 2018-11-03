/***************************************************************************************
Header file : list.cpp
Description : Implementation of methods of structs Node and Result,
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

Result* newResult(){
    Result* temp;

    temp = new Result;
    CHECK_OR_EXIT(temp);

    temp->first = temp->last = NULL;
    temp->nodesNum = temp->totalEntries = 0;

    return temp;
}

void destroyResult(Result* res){
    Node* current;
    Node* temp;

    if(res == NULL) return;

    current = res->first;
    while(current != NULL){
        temp = current;
        current = current->next;
        destroyNode(temp);
    }
    delete res;
}

bool isEmptyResult(Result* res){
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

void insertResult(Result* res,uint32_t rowidA,uint32_t rowidB){

    if(isEmptyResult(res)){ //it's first time
        res->first = newNode();
        res->last = res->first;
        res->nodesNum ++;
    }

    prepareInsertion(res);

    insertNodeEntry(res->last,rowidA,rowidB);
    res->totalEntries ++;

}

/* If there is no space to insert new entry make a new node */
void prepareInsertion(Result* res){
    Node* temp;
    if(res->last->count == ENTRIES_PER_NODE){
        //we need a new node
        temp = newNode();
        res->last->next = temp;
        res->last = temp;
        res->nodesNum ++;
    }
}

uint32_t* getResultEntry(Result* res,uint32_t index){
    return NULL; //to be implemented
}


Node* newNode(){
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

void destroyNode(Node* node){
    delete[] node->buffer;
    delete node;
}

void insertNodeEntry(Node* node,uint32_t rowidA,uint32_t rowidB){
    if(node->count < ENTRIES_PER_NODE){ //just a check for security
        uint32_t offset = node->count * ENTRY_SIZE;
        memcpy(node->buffer + offset,&rowidA,sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(node->buffer + offset,&rowidB,sizeof(uint32_t));
        node->count ++;
    }
}

void printNodeResult(Node* node){
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

uint32_t* getNodeEntry(Node* res,uint32_t index){
    return NULL; //to be implemented
}
