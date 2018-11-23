#include "../singleJoin/result.hpp"
#include "memmap.hpp"
#include "intermediate.hpp"
#include <cstdio>

#ifndef PREDICATES_HPP
#define PREDICATES_HPP

typedef struct Predicate {
    uint64_t relationA;
    uint64_t columnA;
    uint64_t relationB;
    uint64_t columnB;
    char op;
    uint64_t value;
    char predicateType;
} Predicate;

typedef struct SumStruct {
    uint64_t relation;
    uint64_t column;
} SumStruct;

typedef struct QueryInfo {
    uint64_t * relations;
    uint64_t relationsCount;
    Predicate * predicates;
    uint64_t predicatesCount;
    struct SumStruct * sums;
    uint64_t sumsCount;
} QueryInfo;

bool compare(uint64_t x, uint64_t y, char op);

void execute(Predicate * p, uint64_t * relations);

void executeFilter(Predicate * predicate, uint64_t * relations);
void executeJoin(Predicate * predicate, uint64_t * relations);
void executeSelfjoin(Predicate * predicate, uint64_t * relations);

void selfJoinUpdateIR(Result * selfJoinResults);

void makeFilter(QueryInfo * q, int relation, int column, char op, int rv,
                int index);
void makeJoin(QueryInfo * q, int relationA, int columnA, int relationB,
                int columnB, int index);
void makeSelfJoin(QueryInfo * q, int relation, int columnA, int columnB,
                int index);

void printPredicate(Predicate * predicate);
void printFilter(Predicate * predicate);
void printJoin(Predicate * predicate);
void printSelfjoin(Predicate * predicate);

#endif
