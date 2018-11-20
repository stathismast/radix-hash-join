#include <iostream>
#include <stdint.h>
#include <ctime>
#include <cstdlib>
#include <cstring>  // For memcpy

#ifndef STRUCTS_HPP
#define STRUCTS_HPP

struct Column{
    uint64_t * rowid;
    uint64_t * value;
    uint64_t size;
};

#endif //STRUCTS_HPP

Column * newColumn(uint64_t size);
Column * randomColumn(uint64_t size);
Column * serialColumn(uint64_t size);
Column * oddColumn(uint64_t size);
Column * evenColumn(uint64_t size);
void deleteColumn(Column * rel);
void printColumn(Column * rel);
