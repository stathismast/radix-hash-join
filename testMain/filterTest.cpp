#include "../join/parse.hpp"
#include "../join/inputManager.hpp" //for ignoreLine
//global
Relation * r;
uint64_t relationsSize;

Intermediate IR;

void executeQueries() {
    char * line = NULL;
    size_t s = 0;
    while (getline(&line, &s, stdin) > 0) {

         if(ignoreLine(line)){
             continue;
         }

        if (strcmp(line, "F\n") == 0) {
            std::cout << "**End of Batch**" << '\n';
            // free(line);
            continue;
        }

        QueryInfo * queryInfo = parseInput(line);
        for (uint64_t i = 0; i < queryInfo->predicatesCount; i++) {
            // printPredicate(&queryInfo->predicates[i]);
            execute(&queryInfo->predicates[i], queryInfo->relations);
        }

        Column * temp = construct(IR, 0, 0, queryInfo->relations);
        printColumn(temp);
        deleteColumn(temp);

        deleteQueryInfo(queryInfo);
        // free(line);
        // line = NULL;
    }
    free(line);
}

int main(void){
    mapAllData(&r, &relationsSize);

    if( r == NULL ){ //no file found
        std::cout << "No file found. Please try again. Bye!" << '\n';
        return -1;
    }

    // Print the data from a given file
    // if(relationsSize) printData(r[0]);
    std::cout << '\n';

    //execute queries etc
    executeQueries();

    printResult(IR.results, IR.relCount);

    deleteIntermediate(&IR);

    unMapAllData(r, relationsSize);

    return 0;
}
