#include "predicates.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <iostream>

#ifndef PARSE_HPP
#define PARSE_HPP

QueryInfo * parseInput(char * query);

void parseRelations(char * relationsStr, QueryInfo * queryInfo);
void parsePredicates(char * predicatesStr, QueryInfo * queryInfo);
void parseSums(char * sumsStr, QueryInfo * queryInfo);

void findPredicate(char * predicate, QueryInfo * queryInfo, int index);
void splitAt(char * toSplit, char const * delim, char ** left, char ** right);
int countArgs(char * in, char const * delim);

void deleteQueryInfo(QueryInfo * queryInfo);

#endif
