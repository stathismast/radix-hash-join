#include "../singleJoin/result.hpp"
#include "../singleJoin/structs.hpp"
#include "memmap.hpp"

#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP

struct Intermediate{
    uint64_t * results[4];
    uint64_t length;
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
SelfJoinColumn * selfJoinConstructMappedData(Intermediate * IR,
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

uint64_t * singleResultToArray(Result * res);
uint64_t * fastResultToArray(Result * res);
uint64_t * fastResultToArray2(Result * res);
uint64_t * fastResultToArray3(Result * res);
uint64_t * resultToArray(Result * res, uint64_t count, uint64_t index);

bool isEmpty(Intermediate *);

#endif
