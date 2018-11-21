#include <iostream>
#include <sys/mman.h>   // for mmap/munmap

#include "inputManager.hpp"

#ifndef MEMMAP_HPP
#define MEMMAP_HPP

typedef struct Relation{
    uint64_t rows;
    uint64_t cols;
    uint64_t ** data;
} Relation;

uint64_t getFileSize(uint64_t rows, uint64_t cols);
uint64_t * memmap(int fd, uint64_t size);
uint64_t ** convertMap(uint64_t * data, uint64_t rows, uint64_t cols);
Relation mapFile(const char inputFile[]);
void unmapData(Relation rel);
void printData(Relation rel);

void mapAllData(Relation ** r, uint64_t * relationsSize);
void unMapAllData(Relation * r, uint64_t relationsSize);

#endif
