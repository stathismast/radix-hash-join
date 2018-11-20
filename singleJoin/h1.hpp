#include "structs.hpp"

uint64_t h1(uint64_t value);
uint64_t * calculateHistogram(Column * rel);
uint64_t * calculateStartingPositions(uint64_t * histogram);
void printHistogram(uint64_t * histogram);
Column * order(Column * rel, uint64_t * startingPositions);
Column * bucketify(Column * rel,
                  uint64_t ** histogram,
                  uint64_t ** startingPositions);
