#include <iostream>
#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include <cstring>  // For memcpy

#ifndef STRUCTS_HPP
#define STRUCTS_HPP

struct Tuple{
    int32_t rowid;
    int32_t value;
};

struct Relation{
    Tuple * column;
    uint32_t size;
};

#endif //STRUCTS_HPP

Tuple * randomColumn(uint32_t size);
Relation * randomRelation(uint32_t size);
void deleteRelation(Relation * rel);
void printRelation(Relation * rel);
