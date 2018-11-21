#include <cstring>
#include <cstdlib>
#include <ctype.h>
#include <iostream>
#include "parse.hpp"
#include "predicates.hpp"
#include "../join/memmap.hpp"
#include "../singleJoin/result.hpp"

extern Relation * r;
extern uint64_t relationsSize;

QueryInfo * parseInput(FILE * in) {
    char * query = NULL;
    size_t s = 0;
    QueryInfo * queryInfo = new QueryInfo;
    queryInfo->predicatesCount = 0;
    queryInfo->sumsCount = 0;
    // read the query
    getline(&query, &s, in);
    if (query == NULL) {
        std::cout << "Empty Line" << '\n';
        return NULL;
    }
    std::cout << "query: " << query << '\n';
    char* relationsStr = strtok(query, "|");
    char* predicatesStr = strtok(NULL, "|");
    char* sumsStr = strtok(NULL, "|");

    std::cout << "RELATIONS" << '\n';
    parseRelations(relationsStr, queryInfo);
    std::cout << "PREDICATES" << '\n';
    parsePredicates(predicatesStr, queryInfo);
    for (int i = 0; i < queryInfo->predicatesCount; i++) {
        std::cout << "\t";
        printPredicate(&queryInfo->predicates[i]);
        // queryInfo->predicates[i]->printInfo();
    }
    std::cout << "SUMS" << '\n';
    parseSums(sumsStr, queryInfo);
    for (int i = 0; i < queryInfo->sumsCount; i++) {
        std::cout << "\tsum = " << queryInfo->sums[i].relation << \
        "." << queryInfo->sums[i].column << '\n';
    }
    free(query);
    return queryInfo;
}

void parseRelations(char * relationsStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(relationsStr) + 1];
    strncpy(temp, relationsStr, strlen(relationsStr) + 1);
    int relationsCount = 0;
    relationsCount = countArgs(temp, " \t");
    delete[] temp;

    queryInfo->relations = new int[relationsCount];

    queryInfo->relations[0] = atoi(strtok(relationsStr, " \t"));
    for (int i = 1; i < relationsCount; i++) {
        queryInfo->relations[i] = atoi(strtok(NULL, " \t"));
    }

    // print the relations of the query
    for (int i = 0; i < relationsCount; i++) {
        std::cout << "\trelation = " << queryInfo->relations[i] << '\n';
    }
}

void parsePredicates(char * predicatesStr, QueryInfo * queryInfo) {
    char * temp = new char[strlen(predicatesStr) + 1];
    strncpy(temp, predicatesStr, strlen(predicatesStr) + 1);
    queryInfo->predicatesCount = countArgs(temp, "&");
    delete[] temp;

    queryInfo->predicates = new Predicate[queryInfo->predicatesCount];

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
    queryInfo->sumsCount = countArgs(temp, " \t");
    delete[] temp;

    queryInfo->sums = new SumStruct[queryInfo->sumsCount];

    char * relationStr, * columnStr;
    char * sum = strtok(sumsStr, " \t");
    splitAt(sum, ".", &relationStr, &columnStr);
    queryInfo->sums[0].relation = atoi(relationStr);
    queryInfo->sums[0].column = atoi(columnStr);
    for (int i = 1; i < queryInfo ->sumsCount; i++) {
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
        splitAt(leftStr, ".", &columnStr, &relationStr);
        makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '<', atoi(rightStr), index);
        // queryInfo->predicates[index] = new Filter(atoi(relationStr), atoi(columnStr), '<', atoi(rightStr) );
    } else if (strstr(predicate, ">") != NULL) {
        // split the predicate in the number on the right and the relation and
        // column on the left
        splitAt(predicate, ">", &leftStr, &rightStr);
        char * relationStr, * columnStr;
        splitAt(leftStr, ".", &columnStr, &relationStr);
        makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '>', atoi(rightStr), index);
        // queryInfo->predicates[index] = new Filter(atoi(relationStr), atoi(columnStr), '>', atoi(rightStr) );
    } else if (strstr(predicate, "=") != NULL) {
        // split the predicate in the number on the right and the relation and
        // column on the left
        splitAt(predicate, "=", &leftStr, &rightStr);
        // if there is no dot in the right part it means it's just a number so
         // it's an equality filter
        if (strstr(rightStr + 1, ".") == NULL) {
            char * relationStr, * columnStr;
            splitAt(leftStr, ".", &columnStr, &relationStr);
            makeFilter(queryInfo, atoi(relationStr), atoi(columnStr), '=', atoi(rightStr), index);
            // queryInfo->predicates[index] = new Filter(atoi(relationStr), atoi(columnStr), '=', atoi(rightStr) );
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
                // queryInfo->predicates[index] = new SelfJoin(relationA, columnA, columnB);
            } else {
                makeJoin(queryInfo, relationA, columnA, relationB, columnB, index);

                // queryInfo->predicates[index] = new Join(relationA, columnA, relationB, columnB);
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

bool compare(uint64_t x, uint64_t y, char op) {
    if (op == '<')
        return x < y;
    else if (op == '>')
        return x > y;
    else
        return x == y;

}

void executeFilter(Predicate * predicate) {
    // if(relationsSize) printData(r[relation]);
    Relation rel = r[predicate->relationA];
    Result * res = newResult();
    int column = predicate->columnA;
    uint64_t value = predicate->value;
    char op = predicate->op;
    // make the list
    for (uint64_t i = 0; i < rel.rows; i++) {
        if (compare(rel.data[column][i], value, op) ) {
            // std::cout << rel.data[column][i] << '\n';
            insertSingleResult(res, i);
        }
    }
    printSingleResult(res);
}

void executeJoin(Predicate * predicate) {
    std::cout << "Join" << '\n';
}

void executeSelfjoin(Predicate * predicate) {
    std::cout << "SelfJoin" << '\n';
}

void makeFilter(QueryInfo * q, int relation, int column, char op, int rv, int index) {
    q->predicates[index].op = op;
    q->predicates[index].value = (uint64_t) rv;
    q->predicates[index].relationA= (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) column;
    q->predicates[index].predicateType = 1;
}

void makeJoin(QueryInfo * q, int relationA, int columnA, int relationB, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relationA;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].relationB = (uint64_t) relationB;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = 2;
}

void makeSelfJoin(QueryInfo * q, int relation, int columnA, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = 3;
}

void execute(Predicate * predicate) {
    if (predicate->predicateType == 1) {
        executeFilter(predicate);
    } else if (predicate->predicateType == 2) {
        executeJoin(predicate);
    } else {
        executeSelfjoin(predicate);
    }
}

void printPredicate(Predicate * predicate) {
    if (predicate->predicateType == 1) {
        printFilter(predicate);
    } else if (predicate->predicateType == 2) {
        printJoin(predicate);
    } else {
        printSelfjoin(predicate);
    }
}


void printFilter(Predicate * predicate) {
    std::cout << "Filter ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " " << predicate->op << " " << predicate->value << '\n';
}
void printJoin(Predicate * predicate) {
    std::cout << "Join: ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " = " << predicate->relationB << "." << predicate->columnB << '\n';
}
void printSelfjoin(Predicate * predicate) {
    std::cout << "SelfJoin ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " = " << predicate->relationA << "." << predicate->columnB << '\n';
}

void deleteQueryInfo(QueryInfo * queryInfo) {
    delete[] queryInfo->relations;
    delete[] queryInfo->predicates;
    delete[] queryInfo->sums;
    delete queryInfo;
}