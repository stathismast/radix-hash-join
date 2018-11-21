#include "join/parse.hpp"

//global
Relation * r;
uint64_t relationsSize;

Intermediate intermediate;

int main(void){
    mapAllData(&r, &relationsSize);

    // Print the data from a given file
    // if(relationsSize) printData(r[0]);

    //execute queries etc
    QueryInfo * queryInfo = parseInput(stdin);
    for (int i = 0; i < queryInfo->predicatesCount; i++) {
        printPredicate(&queryInfo->predicates[i]);
        execute(&queryInfo->predicates[i]);
    }

    printResult(intermediate.results, intermediate.relCount);

    deleteIntermediate(&intermediate);

    deleteQueryInfo(queryInfo);
    unMapAllData(r, relationsSize);

    return 0;
}
