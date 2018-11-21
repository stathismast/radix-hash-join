#include "../singleJoin/result.hpp"
#include "../singleJoin/structs.hpp"

struct Intermediate{
    Result * results;
    uint64_t relCount;
    uint64_t * relations;
};

Column * resultToColumn(Result * res, uint64_t col, uint64_t entryCount);
void deleteIntermediate(Intermediate * im);
