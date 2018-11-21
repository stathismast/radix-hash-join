#include "../singleJoin/result.hpp"
#include "../join/memmap.hpp"
#include <cstdio>

#ifndef PARSE_HPP
#define PARSE_HPP

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
    int relation;
    int column;
} SumStruct;

typedef struct QueryInfo {
    int * relations;
    Predicate * predicates;
    int predicatesCount;
    struct SumStruct * sums;
    int sumsCount;
} QueryInfo;

QueryInfo * parseInput(FILE * in);

void parseRelations(char * relationsStr, QueryInfo * queryInfo);
void parsePredicates(char * predicatesStr, QueryInfo * queryInfo);
void parseSums(char * sumsStr, QueryInfo * queryInfo);

void findPredicate(char * predicate, QueryInfo * queryInfo, int index);
void splitAt(char * toSplit, char const * delim, char ** left, char ** right);
int countArgs(char * in, char const * delim);

void deleteQueryInfo(QueryInfo * queryInfo);

void execute(Predicate * p);

void executeFilter(QueryInfo * q, int index);
void executeJoin(Predicate * predicate);
void executeSelfjoin(Predicate * predicate);

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
