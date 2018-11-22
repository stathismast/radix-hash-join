#include "../singleJoin/result.hpp"
#include "../singleJoin/structs.hpp"
#include "memmap.hpp"

struct Intermediate{
    Result * results;
    uint64_t relCount;
    uint64_t * relations;
};

Column * resultToColumn(Result * res, uint64_t col, uint64_t entryCount);
Column * construct(Intermediate IR,
                        uint64_t colIR,
                        uint64_t colRel,
                        uint64_t * queryRelations);
void deleteIntermediate(Intermediate * im);
