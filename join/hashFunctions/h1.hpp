#include "../dataStructures/structs.hpp"

int32_t h1(int32_t value);
uint32_t * calculateHistogram(Relation * rel);
uint32_t * calculateStartingPositions(uint32_t * histogram);
void printHistogram(uint32_t * histogram);
Tuple * order(Relation * rel, uint32_t * startingPositions);
Tuple * bucketify(Relation * rel,
                  uint32_t ** histogram,
                  uint32_t ** startingPositions);
