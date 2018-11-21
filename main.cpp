#include "join/memmap.hpp"
#include "join/parse.hpp"
#include "join/intermediate.hpp"

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
        for (int i = 0; i < queryInfo->predicatesCount; i++) {
            // printPredicate(&queryInfo->predicates[i]);
            // execute(&queryInfo->predicates[i]);
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

    unMapAllData(r, relationsSize);

    return 0;
}
