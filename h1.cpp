#include "h1.hpp"

// Number of least significant bits that are used in the hash function
#define BITS 3

// Hash fucntion
int32_t h1(int32_t value){
    return value & ((1 << BITS) - 1);
}

// Takes a relation & returns an array with the num of tuples in each bucket
uint32_t * calculateHistogram(relation * rel){
    int numberOfBuckets = 1 << BITS;

    // Create and initialize histogram
    uint32_t * histogram = new uint32_t[numberOfBuckets];
    for(uint32_t i=0; i<numberOfBuckets; i++)
        histogram[i] = 0;
    
    // Add up the number of tuples in each bucket
    for(uint32_t i=0; i<rel->size; i++)
        histogram[h1(rel->column[i].value)]++;
    return histogram;
}

// Calculate starting position of every bucket
uint32_t * calculateStartingPositions(uint32_t * histogram){
    int numberOfBuckets = 1 << BITS;

    // Create and nitialize starting positions array
    uint32_t * startingPositions = new uint32_t[numberOfBuckets];
    startingPositions[0] = 0;

    // Calculate and store the starting position of each bucket
    for(uint32_t i=1; i<numberOfBuckets; i++)
        startingPositions[i] = startingPositions[i-1] + histogram[i-1];
    return startingPositions;
}

// Print a given histogram (works with every uint32_t array of size = 1<<BITS)
void printHistogram(uint32_t * histogram){
    int numberOfBuckets = 1 << BITS;
    for(uint32_t i=0; i<numberOfBuckets; i++)
        std::cout << i << ". " << histogram[i] << std::endl;
    std::cout << std::endl;
}

// Create an array of tuples ordered by buckets
tuple * order(relation * rel, uint32_t * startingPositions){
    tuple * ordered = new tuple[rel->size];
    for(uint32_t i=0; i<rel->size; i++){
        int32_t val = rel->column[i].value;

        // Store value & rowid of tuple in the appropriate position
        ordered[startingPositions[h1(val)]].value = val;
        ordered[startingPositions[h1(val)]].rowid = rel->column[i].rowid;

        // Increment starting position of the bucket since we just added to it
        startingPositions[h1(val)]++;
    }
    return ordered;
}

tuple * bucketify(relation * rel){
    uint32_t * histogram = calculateHistogram(rel);
    uint32_t * startingPositions = calculateStartingPositions(histogram);
    tuple * ordered = order(rel,startingPositions);
    
    delete[] histogram;
    delete[] startingPositions;
    
    return ordered;
}
