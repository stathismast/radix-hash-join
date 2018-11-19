#include "structs.hpp"

uint64_t h2(uint64_t value, uint64_t start);
uint64_t nextPrime(uint64_t start);
bool isPrime(uint64_t i);

void tesNextPrime(uint64_t upTo);

void bucketify2 (Tuple * rel,
                    uint64_t bucket_size,
                    uint64_t start,
                    uint64_t ** bucket_array,
                    uint64_t ** chain_array);
