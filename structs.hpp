#include <iostream>
#include <stdint.h>
#include <ctime>
#include <cstdlib>

struct tuple{
    int32_t rowid;
    int32_t value;
};

struct relation{
    tuple  *column;
    uint32_t size;
};

tuple * randomColumn(uint32_t size);
relation * randomRelation(uint32_t size);
void printRelation(relation * rel);
