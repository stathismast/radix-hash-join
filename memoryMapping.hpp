/***************************************************************************************
Header file : memoryMapping.hpp
Description : Declaration of structs RelationMap and RelationInfo which are used to
              represent the structs which contains the data of all ralations.
****************************************************************************************/
#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <fstream>

struct Relation{
    uint64_t colsNum;
    uint64_t rowsNum;
    uint64_t **data; //data[col][row]
};

struct RelationMap{
    uint64_t size;
    Relation ** relations;
};

RelationMap * newRelationMap(uint64_t size);
void deleteRelationMap(RelationMap * relMap);
void printRelMap(RelationMap * relMap,std::ofstream * dest);
void insertRelation(RelationMap * relMap,uint64_t index,Relation * rel);

Relation * newRelation(uint64_t colsNum,uint64_t rowsNum);
void deleteRelation(Relation * rel);
void printRelation(Relation * rel,std::ofstream * dest);
void setColData(Relation * rel,uint64_t index,uint64_t * start);

#endif /* MAP_H */
