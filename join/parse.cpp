#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include "parse.hpp"

// uint64_t * queryRelations;

void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

// determine whether or not a given value is in the given uint64_t array
bool inArray(uint64_t value, uint64_t * array, uint64_t arrayLength){
    for(uint64_t i=0; i<arrayLength; i++)
        if(value == array[i])
            return 1;
    return 0;

}

void reOrderPredicates(QueryInfo * queryInfo){
    Predicate * predicates  = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;

    if(count == 0) return;

    uint64_t * vIntermediate = new uint64_t[queryInfo->relationsCount];
    uint64_t viStart = 0;
    uint64_t viEnd = 0;

    uint64_t orderedCount = 0;

    // Find the filter and bring it at the first position
    for(uint64_t i=0; i<count; i++){
        if(predicates[i].predicateType == FILTER){
            swapPredicates(&predicates[i], &predicates[orderedCount]);
            vIntermediate[viEnd] = predicates[orderedCount].relationA;
            viEnd++;
            orderedCount++;
        }
    }

    while(viStart != viEnd){

        // Prioritize self join executions
        for(uint64_t i=orderedCount; i<count; i++){
            // If the next relation in intermediate is included in a self join
            if(vIntermediate[viStart] == predicates[i].relationA &&
                predicates[i].predicateType == SELFJOIN){

                // Bring that predicate up to the next spot
                swapPredicates(&predicates[i], &predicates[orderedCount]);

                // Increase the number of ordered relations
                orderedCount++;
            }
        }

        for(uint64_t i=orderedCount; i<count; i++){

            // If the next relation in intermediate is included in a join
            if(vIntermediate[viStart] == predicates[i].relationA){

                // Bring that predicate up to the next spot
                swapPredicates(&predicates[i], &predicates[orderedCount]);

                // Add new relation into vIntermediate if it isn't already there
                if(predicates[i].predicateType == JOIN &&
                   !inArray(predicates[orderedCount].relationB, vIntermediate, viEnd)){
                        vIntermediate[viEnd] = predicates[orderedCount].relationB;
                        viEnd++;
                }

                // Increase the number of ordered relations
                orderedCount++;
            }

            // If this is certainly not a self join
            // And if the next relation in intermediate is included in a join
            if(predicates[i].predicateType == JOIN &&
               vIntermediate[viStart] == predicates[i].relationB){

                // Bring that predicate up to the next spot
                swapPredicates(&predicates[i], &predicates[orderedCount]);

                // Add new relation into vIntermediate if it isn't already there
                if(!inArray(predicates[orderedCount].relationA, vIntermediate, viEnd)){
                        vIntermediate[viEnd] = predicates[orderedCount].relationA;
                        viEnd++;
                }

                // Increase the number of ordered relations
                orderedCount++;

            }
        }
        viStart++;
    }

    delete[] vIntermediate;
}

QueryInfo * parseInput(char * query) {
    QueryInfo * queryInfo = new QueryInfo;
    queryInfo->predicatesCount = 0;
    queryInfo->sumsCount = 0;
    std::cout << "query: " << query << '\n';
    char* relationsStr = strtok(query, "|");
    char* predicatesStr = strtok(NULL, "|");
    char* sumsStr = strtok(NULL, "|");

    // std::cout << "RELATIONS" << '\n';
    parseRelations(relationsStr, queryInfo);

    // std::cout << "PREDICATES" << '\n';
    parsePredicates(predicatesStr, queryInfo);
    // for (uint64_t i = 0; i < queryInfo->predicatesCount; i++) {
    //     std::cout << "\t";
    //     printPredicate(&queryInfo->predicates[i]);
    // }

    // std::cout << "SUMS" << '\n';
    parseSums(sumsStr, queryInfo);
    // for (uint64_t i = 0; i < queryInfo->sumsCount; i++) {
    //     std::cout << "\tsum = " << queryInfo->sums[i].relation
    //     << "." << queryInfo->sums[i].column << '\n';
    // }


    reOrderPredicates(queryInfo);
    return queryInfo;
}

void parseRelations(char * relationsStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(relationsStr) + 1];
    strncpy(temp, relationsStr, strlen(relationsStr) + 1);
    uint64_t relationsCount = 0;
    relationsCount = countArgs(temp, " \t");
    queryInfo->relationsCount = relationsCount;
    delete[] temp;

    queryInfo->relations = new uint64_t[relationsCount];

    queryInfo->relations[0] = atoi(strtok(relationsStr, " \t"));
    for (uint64_t i = 1; i < relationsCount; i++) {
        queryInfo->relations[i] = atoi(strtok(NULL, " \t"));
    }

    // print the relations of the query
    // for (uint64_t i = 0; i < relationsCount; i++) {
    //     std::cout << "\trelation = " << queryInfo->relations[i] << '\n';
    // }
}

void parsePredicates(char * predicatesStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(predicatesStr) + 1];
    strncpy(temp, predicatesStr, strlen(predicatesStr) + 1);
    queryInfo->predicatesCount = countArgs(temp, "&");
    delete[] temp;

    queryInfo->predicates = new Predicate[queryInfo->predicatesCount];

    char * predicate = strtok(predicatesStr, "&");
    findPredicate(predicate, queryInfo, 0);
    for (uint64_t i = 1; i < queryInfo->predicatesCount; i++) {
        predicate = strtok(NULL, "&");
        findPredicate(predicate, queryInfo, i);
    }
}

void parseSums(char * sumsStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(sumsStr) + 1];
    strncpy(temp, sumsStr, strlen(sumsStr) + 1);
    queryInfo->sumsCount = countArgs(temp, " \t");
    delete[] temp;

    queryInfo->sums = new SumStruct[queryInfo->sumsCount];

    char * relationStr, * columnStr;
    char * sum = strtok(sumsStr, " \t");
    splitAt(sum, ".", &relationStr, &columnStr);
    queryInfo->sums[0].relation = atoi(relationStr);
    queryInfo->sums[0].column = atoi(columnStr);
    for (uint64_t i = 1; i < queryInfo ->sumsCount; i++) {
        char * sum = strtok(NULL, " \t");
        splitAt(sum, ".", &relationStr, &columnStr);
        queryInfo->sums[i].relation = atoi(relationStr);
        queryInfo->sums[i].column = atoi(columnStr);
    }
}

void findPredicate(char * predicate, QueryInfo * queryInfo, int index) {
    char * leftStr, * rightStr;
    // check what kinf of predicate it is
    if (strstr(predicate, "<") != NULL) {
        // split the predicate in the number on the right and the relation and
        // column on the left
        splitAt(predicate, "<", &leftStr, &rightStr);
        char * relationStr, * columnStr;
        splitAt(leftStr, ".", &relationStr, &columnStr);
        makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '<', \
                    atoi(rightStr), index);
    } else if (strstr(predicate, ">") != NULL) {
        // split the predicate in the number on the right and the relation and
        // column on the left
        splitAt(predicate, ">", &leftStr, &rightStr);
        char * relationStr, * columnStr;
        splitAt(leftStr, ".", &relationStr, &columnStr);
        makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '>', \
                    atoi(rightStr), index);
    } else if (strstr(predicate, "=") != NULL) {
        // split the predicate in the number on the right and the relation and
        // column on the left
        splitAt(predicate, "=", &leftStr, &rightStr);

        // if there is no dot in the right part it means it's just a number so
         // it's an equality filter
        if (strstr(rightStr + 1, ".") == NULL) {
            char * relationStr, * columnStr;

            splitAt(leftStr, ".", &relationStr, &columnStr);
            makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '=', \
                        atoi(rightStr), index);
        } else {
            char * relationStr, * columnStr;
            // split the left part of the predicate in the relation
             // and the column

            splitAt(leftStr, ".", &relationStr, &columnStr);
            int relationA = atoi(relationStr);
            int columnA = atoi(columnStr);

            // same as above for the left part
            splitAt(rightStr, ".", &relationStr, &columnStr);
            int relationB = atoi(relationStr);
            int columnB = atoi(columnStr);

            // if the join is between the same relation we have a self join
            // and it will be treated differenlty than normal join
            if (relationA == relationB) {
                makeSelfJoin(queryInfo, relationA, columnA, columnB, index);
            } else {
                makeJoin(queryInfo, relationA, columnA, relationB, columnB, \
                            index);
            }
        }
    }
}

int countArgs(char * in, char const * delim) {
    int count = 0;
    if (strtok(in, delim) == NULL) {
        count++;
    } else {
        count++;
        while (strtok(NULL, delim) != NULL) {
            count++;
        }
    }
    return count;
}

void splitAt(char * toSplit, char const * delim, char ** left, char ** right) {
    char* splitPos = strstr(toSplit, delim);
    *right = splitPos + 1;
    *splitPos = '\0';
    *left = toSplit;
}
