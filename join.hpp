#include "h1.hpp"
#include "h2.hpp"
#include "result.hpp"

void join(relation * A, relation * B);

void compare(tuple * orderedBig, tuple * orderedSmall, uint32_t bucketSizeSmall,\
    uint32_t startIndexSmall, uint32_t bucketSizeBig, uint32_t startIndexBig, \
    uint32_t * bucketArray, uint32_t * chainArray, Result ** result, char flag);

void checkEquals(tuple * valueA, int hash_value, tuple * orderedSmall, \
    uint32_t bucketSizeBig, uint32_t startIndexBig, uint32_t * bucketArray, \
    uint32_t * chainArray, Result ** result, char flag, int rowIdBig);
