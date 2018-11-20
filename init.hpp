/***************************************************************************************
Header file : init.hpp
Description : Declaration of functions which are used to map the relations
              from disk to memory.
****************************************************************************************/
#ifndef INIT_H
#define INIT_H

#include "memoryMapping.hpp"
#include <cstring>
#define HEADER_SIZE (sizeof(uint64_t) + sizeof(uint64_t))

RelationMap * init();
Relation* initRelation(char * relFile);
void destroy(RelationMap * relMap);
void printRelation(RelationMap * relMap,uint64_t index,std::ofstream * dest);

#endif /* INIT_H */
