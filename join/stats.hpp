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
void updateStats(uint64_t rel, uint64_t col, Stats newStats);
