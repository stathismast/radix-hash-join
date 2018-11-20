#include "memmap.hpp"

// Returns the size of a file based on the values of the header
uint64_t getFileSize(uint64_t rows, uint64_t cols){
    return sizeof(uint64_t) * rows * cols + 2 * sizeof(uint64_t);
}

// A wrapper for mmap. Only requires the file descriptor and a total size
uint64_t * memmap(int fd, uint64_t size){
    return (uint64_t*) mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd, 0);
}

// Takes an input of a 1D array and returns an indexed 2D array
uint64_t ** convertMap(uint64_t * data, uint64_t rows, uint64_t cols){
    uint64_t ** index = new uint64_t*[cols];

    for(uint64_t i=0; i<cols; i++)
        index[i] = data + i * rows;
    
    return index;
}

// Given a file name, this function will create a return a Relation struct
// containing the number of columns, rows and all the data in a 2D array
Relation mapFile(char inputFile[]){
    Relation rel;

    // Open input file
    int fd = open(inputFile, O_RDONLY);

    // Read the number of columns and rows
    uint64_t * header = memmap(fd, 128);
    rel.rows = header[0];
    rel.cols = header[1];

    // Calculate total length and map the whole file
    uint64_t size = getFileSize(rel.rows, rel.cols);
    uint64_t * data = memmap(fd, size);
    rel.data = convertMap(data+2, rel.rows, rel.cols);

    close(fd);
    return rel;
}

// Unmaps data of given relation and deletes the 2D array
void unmapData(Relation rel){
    munmap(*rel.data - 2, getFileSize(rel.rows, rel.cols));
    delete[] rel.data;
}

void printData(Relation rel){
    for(int i=0; i<rel.rows; i++){
        std::cout << rel.data[0][i] << "|";
        std::cout << rel.data[1][i] << "|";
        std::cout << rel.data[2][i] << "|" << std::endl;
    }
}
