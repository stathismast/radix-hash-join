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
Tuple * serialColumn(uint32_t size);
Relation * serialRelation(uint32_t size);
Tuple * oddColumn(uint32_t size);
Relation * oddRelation(uint32_t size);
Tuple * evenColumn(uint32_t size);
Relation * evenRelation(uint32_t size);
void deleteRelation(Relation * rel);
void printRelation(Relation * rel);
