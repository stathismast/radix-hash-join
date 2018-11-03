#include "structs.hpp"

int32_t h2(int32_t value, uint32_t start);
uint32_t nextPrime(uint32_t start);
char isPrime(uint32_t i);

void test_nextPrime(uint32_t upTo);

void bucketify2 (relation * rel,
                    uint32_t bucket_size,
                    uint32_t start,
                    uint32_t ** bucket_array,
                    uint32_t ** chain_array);
