#include "h1.hpp"
#include "h2.hpp"
#include "result.hpp"

Result * join(Relation * A, Relation * B);

void compare(Tuple * orderedBig, Tuple * orderedSmall, uint64_t bucketSizeSmall,\
    uint64_t startIndexSmall, uint64_t bucketSizeBig, uint64_t startIndexBig, \
    uint64_t * bucketArray, uint64_t * chainArray, Result * result, bool flag);

void checkEquals(Tuple * valueA, int hash_value, Tuple * orderedSmall, \
    uint64_t startIndexBig, uint64_t * bucketArray, uint64_t * chainArray, \
    Result * result, bool flag, int rowIdBig);

int naiveJoin(Relation * A, Relation * B);
