#include <iostream>
#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include <cstring>  // For memcpy

#ifndef STRUCTS_HPP
#define STRUCTS_HPP

struct Relation{
    uint64_t * rowid;
    uint64_t * value;
    uint64_t size;
};

#endif //STRUCTS_HPP

Relation * newRelation(uint64_t size);
Relation * randomRelation(uint64_t size);
Relation * serialRelation(uint64_t size);
Relation * oddRelation(uint64_t size);
Relation * evenRelation(uint64_t size);
void deleteRelation(Relation * rel);
void printRelation(Relation * rel);
