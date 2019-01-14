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

// This function calculates the requiredstatistics (l,u,f,d)
// for every column in a given relation
void calculateStats(Relation * rel){
    // Allocate arrays for statistics of each column
    rel->l = new double[rel->cols];
    rel->u = new double[rel->cols];
    rel->f = new double[rel->cols];
    rel->d = new double[rel->cols];

    for(uint64_t i=0; i<rel->cols; i++){

        // Calculate min and max values
        uint64_t max = rel->data[i][0];
        uint64_t min = rel->data[i][0];
        for(uint64_t j=1; j<rel->rows; j++){
            if(rel->data[i][j] > max) max = rel->data[i][j];
            if(rel->data[i][j] < min) min = rel->data[i][j];
        }
        rel->l[i] = (double) min;
        rel->u[i] = (double) max;

        // Calculate total number of values
        rel->f[i] = rel->rows;

        // Initialize and calculate boolean array
        uint64_t arraySize = rel->u[i] - rel->l[i] + 1;
        if(arraySize > 50000000) arraySize = 50000000;
        bool * tempArray = new bool[arraySize];
        for(uint64_t j=0; j<arraySize; j++){
            tempArray[j] = false;
        }
        for(uint64_t j=0; j<rel->rows; j++){
            tempArray[(rel->data[i][j] - (uint64_t)rel->l[i]) % arraySize] = true;
        }

        // Calculate the total number of different values
        rel->d[i] = 0;
        for(uint64_t j=0; j<arraySize; j++){
            if(tempArray[j] == true){
                rel->d[i]++;
            }
        }
        //
        // std::cout << i << ": l=" << rel->l[i]
        //                << "  u=" << rel->u[i]
        //                << "  f=" << rel->f[i]
        //                << "  d=" << rel->d[i] << "\n";

        delete[] tempArray;
    }

    // std::cout << "\n";
}

// Given a file name, this function will create a return a Relation struct
// containing the number of columns, rows and all the data in a 2D array
Relation mapFile(const char inputFile[]){
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

    calculateStats(&rel);

    close(fd);
    return rel;
}

// Unmaps data of given relation and deletes the 2D array
void unmapData(Relation rel){
    munmap(*rel.data - 2, getFileSize(rel.rows, rel.cols));
    delete[] rel.data;
    delete[] rel.l;
    delete[] rel.u;
    delete[] rel.f;
    delete[] rel.d;
}

void printData(Relation rel){
    for(uint64_t i=0; i<rel.rows; i++){
        for(uint64_t j=0; j<rel.cols; j++){
            std::cout << rel.data[j][i] << "|";
        }
        std::cout << std::endl;
    }
}

void mapAllData(Relation ** r, uint64_t * relationsSize){
    char ** inputFiles = getInputFiles(relationsSize);

    if( inputFiles == NULL ){ //no file found
        r = NULL;
        return;
    }

    // Allocate an array of relations according to the number of input files
    *r = new Relation[*relationsSize];

    // Map every file to memory
    for( uint64_t i = 0; i < *relationsSize; i++ ){
        (*r)[i] = mapFile(inputFiles[i]);
    }

    // Deallocate memory used for file paths
    for(uint64_t i=0; i<*relationsSize; i++){
        // std::cout << inputFiles[i] << std::endl;
        delete[] inputFiles[i];
    }
    delete[] inputFiles;
}

void unMapAllData(Relation * r, uint64_t relationsSize){
    // Unmap data and delete 2D index
    for(uint64_t i=0; i<relationsSize; i++){
        unmapData(r[i]);
    }
    delete[] r;
}
