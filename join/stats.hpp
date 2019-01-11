#include <stdint.h>     // for uint64_t
#include <math.h>

#include "memmap.hpp"

typedef struct Stats{
    double l;
    double u;
    double f;
    double d;
} Stats;

void printStats(uint64_t rel);

void createStats();
void deleteStats();
void initializeStats();

void updateEqualFilterStats(uint64_t rel, uint64_t col, uint64_t k);
void updateLessFilterStats(uint64_t rel, uint64_t col, uint64_t k);
void updateGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k);
void updateSelfJoinStats(uint64_t rel, uint64_t colA, uint64_t colB);
void updateJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);
void updateStats(uint64_t rel, uint64_t col, Stats newStats);

Stats evalEqualFilterStats(uint64_t rel, uint64_t col, uint64_t k);
Stats evalLessFilterStats(uint64_t rel, uint64_t col, uint64_t k);
Stats evalGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k);
Stats evalelfJoinStats(uint64_t rel, uint64_t colA, uint64_t colB);
Stats evalJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);
