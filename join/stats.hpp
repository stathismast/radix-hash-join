#include <stdint.h>     // for uint64_t
#include <math.h>

#include "memmap.hpp"

typedef struct Stats{
    double l;
    double u;
    double f;
    double d;
} Stats;

void createStats();
void deleteStats();
void initializeStats();
void equalFilter(uint64_t rel, uint64_t col, uint64_t k);
void lessFilterStats(uint64_t rel, uint64_t col, uint64_t k);
void greaterFilterStats(uint64_t rel, uint64_t col, uint64_t k);
void selfJoinStats(uint64_t rel, uint64_t colA, uint64_t colB);
void joinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);
void updateStats(uint64_t rel, uint64_t col, Stats newStats);
