#include "predicates.hpp"
#include <cstdio>

#ifndef PARSE_HPP
#define PARSE_HPP

struct sumStruct {
    int relation;
    int column;
};

struct QueryInfo {
    int * relations;
    Predicate ** predicates;
    int predicatesCount;
    struct sumStruct * sums;
    int sumsCount;
};

QueryInfo * parseInput(FILE * in);

void parseRelations(char * relationsStr, QueryInfo * queryInfo);
void parsePredicates(char * predicatesStr, QueryInfo * queryInfo);
void parseSums(char * sumsStr, QueryInfo * queryInfo);

void findPredicate(char * predicate, QueryInfo * queryInfo, int index);

#endif
