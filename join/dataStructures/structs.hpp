#include <iostream>
#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include <cstring>  // For memcpy

#ifndef STRUCTS_HPP
#define STRUCTS_HPP

struct tuple{
    int32_t rowid;
    int32_t value;
};

struct relation{
    tuple  *column;
    uint32_t size;
};

#endif //STRUCTS_HPP

tuple * randomColumn(uint32_t size);
relation * randomRelation(uint32_t size);
void deleteRelation(relation * rel);
void printRelation(relation * rel);

relation * segFaultTupleA();
relation * segFaultTupleB();
