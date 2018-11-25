#include "../singleJoin/result.hpp"
#include "../singleJoin/structs.hpp"
#include "memmap.hpp"

struct Intermediate{
    Result * results;
    uint64_t relCount;
    uint64_t * relations;
};

struct SelfJoinColumn{
    uint64_t * rowid;
    uint64_t * valueA;
    uint64_t * valueB;
    uint64_t size;
};

Column * resultToColumn(Result * res, uint64_t col, uint64_t entryCount);
Column * construct(Intermediate * IR,
                        uint64_t colIR,
                        uint64_t colRel,
                        uint64_t * queryRelations);
SelfJoinColumn * selfJoinConstruct(Intermediate * IR,
                                   uint64_t relation,
                                   uint64_t relColumnA,
                                   uint64_t relColumnB,
                                   uint64_t * queryRelations);
Column * constructMappedData(uint64_t relIndex,
                             uint64_t column,
                             uint64_t * queryRelations);
void deleteIntermediate(Intermediate * im);
void deleteSJC(SelfJoinColumn * sjc);

bool isInIntermediate(Intermediate * intermediate, uint64_t relIndex);
