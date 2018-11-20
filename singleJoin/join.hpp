#include "h1.hpp"
#include "h2.hpp"
#include "result.hpp"

Result * join(Column * A, Column * B);

void compare(Column * orderedBig,
            Column * orderedSmall,
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
                 Column * orderedSmall,
                 uint64_t startIndexSmall,
                 uint64_t * bucketArray,
                 uint64_t * chainArray,
                 Result * result,
                 bool flag,
                 int rowIdBig);

int naiveJoin(Column * A, Column * B);
