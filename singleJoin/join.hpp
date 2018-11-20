#include "h1.hpp"
#include "h2.hpp"
#include "result.hpp"

Result * join(Relation * A, Relation * B);

void compare(Relation * orderedBig,
            Relation * orderedSmall,
            uint64_t bucketSizeBig,
            uint64_t startIndexBig,
            uint64_t bucketSizeSmall,
            uint64_t startIndexSmall,
            uint64_t * bucketArray,
            uint64_t * chainArray,
            Result * result,
            bool flag);

void checkEquals(uint64_t rowidA,
                 uint64_t valueA,
                 int hash_value,
                 Relation * orderedSmall,
                 uint64_t startIndexSmall,
                 uint64_t * bucketArray,
                 uint64_t * chainArray,
                 Result * result,
                 bool flag,
                 int rowIdBig);

int naiveJoin(Relation * A, Relation * B);
