/***************************************************************************************
Source file : memoryMapping.cpp
Description : Implementation of methods of structs RelationMap and RelationInfo,
              which are used to store and retrieve all the relations.
****************************************************************************************/

#include "memoryMapping.hpp"

#define CHECK_OR_EXIT(value)                                        \
{                                                                   \
    if (value == NULL) {                                            \
        std::cout << "Error at memory allocation." << std::endl;    \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
}

RelationMap * newRelationMap(uint64_t size){
    RelationMap * relMap;

    if(size <= 0) return NULL;
    relMap = new RelationMap;
    CHECK_OR_EXIT(relMap);

    relMap->size = size;
    relMap->relations = new Relation*[size];
    CHECK_OR_EXIT(relMap->relations);

    for( uint64_t i = 0; i < size; i ++ ){
        relMap->relations[i] = NULL;
    }

    return relMap;
}

void deleteRelationMap(RelationMap * relMap){
    if(relMap == NULL) return;

    for( uint64_t i = 0; i < relMap->size; i ++ ){
        if(relMap->relations[i] != NULL){
            deleteRelation(relMap->relations[i]);
        }
    }

    if(relMap->relations != NULL){
        delete[] relMap->relations;
    }

    delete relMap;
}

void printRelFromMap(RelationMap * relMap,uint64_t index,std::ofstream * dest){
    if( index < 0 || index > relMap->size ) return;

    std::cout << "Printing Relation " << index << ":\n";
    printRelation(relMap->relations[index],dest);
}

void insertRelation(RelationMap * relMap,uint64_t index,Relation * rel){
    if( index < 0 || index > relMap->size ) return;

    if( relMap->relations[index] != NULL ) return;

    relMap->relations[index] = rel;
}


Relation * newRelation(uint64_t colsNum,uint64_t rowsNum){
    Relation * rel;

    if( colsNum <= 0 || rowsNum <= 0) return NULL;

    rel = new Relation;
    CHECK_OR_EXIT(rel);

    rel->colsNum = colsNum;
    rel->rowsNum = rowsNum;
    rel->data = new uint64_t*[colsNum];
    CHECK_OR_EXIT(rel->data);

    for( uint64_t i = 0; i < colsNum; i ++ ){
        rel->data[i] = NULL;
    }

    return rel;
}

void deleteRelation(Relation * rel){
    if(rel == NULL) return;

    if( rel->data != NULL ){
        delete[] rel->data;
    }

    delete rel;
}

void printRelation(Relation * rel,std::ofstream * dest){
    uint64_t i,j;
    for( j = 0; j < rel->rowsNum; j ++){ //print in form of a table
        for( i = 0; i < rel->colsNum - 1; i ++ ){
            (*dest) << rel->data[i][j] << "|";
        }
        (*dest) << rel->data[i][j] << "\n";
    }
}

void setColData(Relation * rel,uint64_t index,uint64_t * start){
    if( index < 0 || index > rel->colsNum ) return;

    if( rel->data[index] != NULL ) return;

    //start is a pointer at the begining of the column data
    rel->data[index] = start;
}
