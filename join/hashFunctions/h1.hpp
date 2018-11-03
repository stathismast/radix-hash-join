#include "../dataStructures/structs.hpp"

int32_t h1(int32_t value);
uint32_t * calculateHistogram(relation * rel);
uint32_t * calculateStartingPositions(uint32_t * histogram);
void printHistogram(uint32_t * histogram);
tuple * order(relation * rel, uint32_t * startingPositions);
tuple * bucketify(relation * rel,
                  uint32_t ** histogram,
                  uint32_t ** startingPositions);
