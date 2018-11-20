#include <iostream>
#include <sys/mman.h>   // for mmap/munmap
#include <fcntl.h>      // for O_RDONLY
#include <unistd.h>     // for open/close
#include <stdint.h>     // for uint64_t

typedef struct Relation{
    uint64_t rows;
    uint64_t cols;
    uint64_t ** data;
} Relation;

uint64_t getFileSize(uint64_t rows, uint64_t cols);
uint64_t * memmap(int fd, uint64_t size);
uint64_t ** convertMap(uint64_t * data, uint64_t rows, uint64_t cols);
Relation mapFile(char inputFile[]);
void unmapData(Relation rel);
void printData(Relation rel);
