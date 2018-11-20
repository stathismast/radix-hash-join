#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include "parse.hpp"

QueryInfo * parseInput(FILE * in) {
    char * query = NULL;
    size_t s = 0;
    QueryInfo * queryInfo = new QueryInfo;
    queryInfo->relationsCount = 0;
    queryInfo->predicatesCount = 0;
    queryInfo->sumsCount = 0;
    // read the query
    getline(&query, &s, in);
    // std::cout << "query = " << query << '\n';
    char* relationsStr = strtok(query, "|");
    char* predicatesStr = strtok(NULL, "|");
    char* sumsStr = strtok(NULL, "|");

    std::cout << "relationsStr = " << relationsStr << '\n';
    std::cout << "predicatesStr = " << predicatesStr << '\n';
    std::cout << "sumsStr = " << sumsStr << '\n';
    parseRelations(relationsStr, queryInfo);
    parsePredicates(predicatesStr, queryInfo);
    parseSums(sumsStr, queryInfo);

    std::cout << "relationsCount = " << queryInfo->relationsCount << '\n';
    std::cout << "predicatesCount = " << queryInfo->predicatesCount << '\n';
    std::cout << "sumsCount = " << queryInfo->sumsCount << '\n';

    return queryInfo;

}


void parseRelations(char * relationsStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(relationsStr) + 1];
    strncpy(temp, relationsStr, strlen(relationsStr) + 1);
    if (strtok(temp, " \t") == NULL) {
        queryInfo->relationsCount++;
    } else {
        queryInfo->relationsCount++;
        while (strtok(NULL, " \t") != NULL) {
            queryInfo->relationsCount++;
        }
    }
    delete[] temp;
    queryInfo->relations = new int[queryInfo ->relationsCount];
    queryInfo->relations[0] = atoi(strtok(relationsStr, " \t"));
    for (int i = 1; i < queryInfo ->relationsCount; i++) {
        queryInfo->relations[i] = atoi(strtok(NULL, " \t"));
    }
    for (int i = 0; i < queryInfo ->relationsCount; i++) {
        std::cout << "relation = " << queryInfo->relations[i] << '\n';
    }
}

void parsePredicates(char * predicatesStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(predicatesStr) + 1];
    strncpy(temp, predicatesStr, strlen(predicatesStr) + 1);
    if (strtok(temp, "&") == NULL) {
        queryInfo->predicatesCount++;
    } else {
        queryInfo->predicatesCount++;
        while (strtok(NULL, "&") != NULL) {
            queryInfo->predicatesCount++;
        }
    }
    delete[] temp;
    queryInfo->predicates = new Predicate * [queryInfo->predicatesCount];
    char * predicate = strtok(predicatesStr, "&");
    findPredicate(predicate, queryInfo);
    std::cout << "predicate = " << predicate << '\n';
    for (int i = 1; i < queryInfo->predicatesCount; i++) {
        predicate = strtok(NULL, "&");
        std::cout << "predicate = " << predicate << '\n';
        findPredicate(predicate, queryInfo);
    }

}

void parseSums(char * sumsStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(sumsStr) + 1];
    strncpy(temp, sumsStr, strlen(sumsStr) + 1);
    if (strtok(temp, " \t") == NULL) {
        queryInfo->sumsCount++;
    } else {
        queryInfo->sumsCount++;
        while (strtok(NULL, " \t") != NULL) {
            queryInfo->sumsCount++;
        }
    }
    delete[] temp;
    queryInfo->sums = new sumStruct[queryInfo->sumsCount];
    std::cout << "sum = " << strtok(sumsStr, " \t") << '\n';
    for (int i = 1; i < queryInfo ->sumsCount; i++) {
        // queryInfo->sums[i] = atoi(strtok(NULL, "&"));
        std::cout << "sum = " << strtok(NULL, " \t") << '\n';
    }
}

void findPredicate(char * predicate, QueryInfo * queryInfo, int index) {
    if (strstr(predicate, "<") != NULL) {
        queryInfo->predicates[index] = new Filter();
    } else if (strstr(predicate, "<") != NULL) {
        queryInfo->predicates[index] = new Filter();
    } else {
        // more ifs to find if it's join, slef join or filter
    }
}
