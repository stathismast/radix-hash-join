#include "join/parse.hpp"

//global
Relation * r;
uint64_t relationsSize;

Intermediate intermediate;

void executeQueries() {
    char * line = NULL;
    size_t s = 0;
    while (getline(&line, &s, stdin) > 0) {
        if (strcmp(line, "F\n") == 0) {
            std::cout << "**End of Batch**" << '\n';
            // free(line);
            continue;
        }
        QueryInfo * queryInfo = parseInput(line);
        for (uint64_t i = 0; i < queryInfo->predicatesCount; i++) {
            // printPredicate(&queryInfo->predicates[i]);
            execute(&queryInfo->predicates[i]);
        }
        deleteQueryInfo(queryInfo);
        free(line);
        line = NULL;
    }
    free(line);
}

int main(void){
    mapAllData(&r, &relationsSize);

    // Print the data from a given file
    // if(relationsSize) printData(r[0]);
    std::cout << '\n';

    //execute queries etc
    executeQueries();

    printResult(intermediate.results, intermediate.relCount);

    deleteIntermediate(&intermediate);

    unMapAllData(r, relationsSize);

    return 0;
}
