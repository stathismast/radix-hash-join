#include "h1.hpp"
#include "h2.hpp"
#include "list.hpp"

void join(relation * A, relation * B);

void compare(relation * relA, relation * relB, uint32_t histogramA, \
    uint32_t startingPosA, uint32_t histogramB, uint32_t startingPosB, \
    int * bucket_array, int * chain_array, Result ** result, \
    char fag);

void checkEquals(int32_t valueA, int hash_value, relation * relB, \
    uint32_t bucketSizeB, uint32_t startingPosB, int * bucket_array, \
    int * chain_array, Result ** result, char flag, int rowIdA);
