#include "h1.hpp"

// Number of least significant bits that are used in the hash function
#define BITS 8

uint64_t numberOfBuckets = 1 << BITS;

// Hash fucntion
uint64_t h1(uint64_t value){
    return value & ((1 << BITS) - 1);
}

// Takes a Relation & returns an array with the num of tuples in each bucket
uint64_t * calculateHistogram(Relation * rel){
    // Create and initialize histogram
    uint64_t * histogram = new uint64_t[numberOfBuckets];
    for(uint64_t i=0; i<numberOfBuckets; i++)
        histogram[i] = 0;

    // Add up the number of tuples in each bucket
    for(uint64_t i=0; i<rel->size; i++)
        histogram[h1(rel->value[i])]++;
    return histogram;
}

// Calculate starting position of every bucket
uint64_t * calculateStartingPositions(uint64_t * histogram){
    // Create and nitialize starting positions array
    uint64_t * startingPositions = new uint64_t[numberOfBuckets];
    startingPositions[0] = 0;

    // Calculate and store the starting position of each bucket
    for(uint64_t i=1; i<numberOfBuckets; i++)
        startingPositions[i] = startingPositions[i-1] + histogram[i-1];
    return startingPositions;
}

// Print a given histogram (works with every uint64_t array of size = 1<<BITS)
void printHistogram(uint64_t * histogram){
    for(uint64_t i=0; i<numberOfBuckets; i++)
        std::cout << i << ". " << histogram[i] << std::endl;
    std::cout << std::endl;
}

// Create an array of tuples ordered by buckets
Relation * order(Relation * rel, uint64_t * startingPositions){
    Relation * ordered = newRelation(rel->size);

    uint64_t * offsets = new uint64_t[numberOfBuckets];
    memcpy(offsets, startingPositions, numberOfBuckets * sizeof(uint64_t));

    for(uint64_t i=0; i<rel->size; i++){
        uint64_t val = rel->value[i];

        // Store value & rowid of Tuple in the appropriate position
        ordered->value[offsets[h1(val)]] = val;
        ordered->rowid[offsets[h1(val)]] = rel->rowid[i];

        // Increment starting position of the bucket since we just added to it
        offsets[h1(val)]++;
    }

    delete[] offsets;

    return ordered;
}

// Takes A as input and returns A'
Relation * bucketify(Relation * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions){

    // Calculate histograms
    *histogram = calculateHistogram(rel);

    // Calculate starting position of each bucket
    *startingPositions = calculateStartingPositions(*histogram);

    // Order the given touples bucket by bucket (basically produces A' from A)
    Relation * ordered = order(rel, *startingPositions);

    return ordered;
}
