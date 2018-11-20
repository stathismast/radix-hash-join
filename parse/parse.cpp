#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include "parse.hpp"
#include "../predicates/predicates.hpp"

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
    findPredicate(predicate, queryInfo, 0);
    for (int i = 1; i < queryInfo->predicatesCount; i++) {
        predicate = strtok(NULL, "&");
        findPredicate(predicate, queryInfo, i);
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
    char * sum = strtok(sumsStr, " \t");
    char* dotPos = strstr(sum, ".");
    queryInfo->sums[0].column = atoi(dotPos + 1);
    *dotPos = '\0';
    queryInfo->sums[0].relation = atoi(sum);

    for (int i = 1; i < queryInfo ->sumsCount; i++) {
        char * sum = strtok(NULL, " \t");
        char* dotPos = strstr(sum, ".");
        queryInfo->sums[i].column = atoi(dotPos + 1);
        *dotPos = '\0';
        queryInfo->sums[i].relation = atoi(sum);
    }
}

void findPredicate(char * predicate, QueryInfo * queryInfo, int index) {
    if (strstr(predicate, "<") != NULL) {
        char * opStr = strstr(predicate, "<");
        int rv = atoi(opStr + 1);
        char op = *(opStr);
        *opStr = '\0';
        char * lv = predicate;
        queryInfo->predicates[index] = new Filter(lv, op, rv);
    } else if (strstr(predicate, ">") != NULL) {
        char * opStr = strstr(predicate, ">");
        int rv = atoi(opStr + 1);
        char op = *(opStr);
        *opStr = '\0';
        char * lv = predicate;
        queryInfo->predicates[index] = new Filter(lv, op, rv);
    } else if (strstr(predicate, "=") != NULL) {
        char * opStr = strstr(predicate, "=");
        if (strstr(opStr + 1, ".") == NULL) {
            int rv = atoi(opStr + 1);
            char op = *(opStr);
            *opStr = '\0';
            char * lv = predicate;
            queryInfo->predicates[index] = new Filter(lv, op, rv);
        } else {
            char * rv = opStr + 1;
            *opStr = '\0';
            char * lv = predicate;

            char* dotPos = strstr(lv, ".");
            int columnA = atoi(dotPos + 1);
            *dotPos = '\0';
            int relationA = atoi(lv);

            dotPos = strstr(rv, ".");
            int columnB = atoi(dotPos + 1);
            *dotPos = '\0';
            int relationB = atoi(rv);

            if (relationA == relationB) {
                queryInfo->predicates[index] = new SelfJoin(relationA, columnA, columnB);
            } else {
                queryInfo->predicates[index] = new Join(relationA, columnA, relationB, columnB);
            }
        }
    }
}
