#include "./hashFunctions/h1.hpp"
#include "./hashFunctions/h2.hpp"
#include "./dataStructures/result.hpp"

Result * join(Relation * A, Relation * B);

void compare(Tuple * orderedBig, Tuple * orderedSmall, uint32_t bucketSizeSmall,\
    uint32_t startIndexSmall, uint32_t bucketSizeBig, uint32_t startIndexBig, \
    uint32_t * bucketArray, uint32_t * chainArray, Result * result, bool flag);

void checkEquals(Tuple * valueA, int hash_value, Tuple * orderedSmall, \
    uint32_t startIndexBig, uint32_t * bucketArray, \
    uint32_t * chainArray, Result * result, bool flag, int rowIdBig);

int naiveJoin(Relation * A, Relation * B);
