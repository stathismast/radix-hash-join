#include "join/parse.hpp"
#include "join/inputManager.hpp" //for ignoreLine
#include "join/stats.hpp"
#include "threads/scheduler.hpp"
#include "join/optimizer.hpp"

//global
Relation * r;
uint64_t relationsSize;
Stats ** stats;
JobScheduler * myJobScheduler;
extern uint64_t * histograms[4];
extern uint64_t * psums[4];

// Intermediate IR;

void executeQueries() {
    char * line = NULL;
    size_t s = 0;

    while (getline(&line, &s, stdin) > 0) {
         if(ignoreLine(line)){
             continue;
         }

        if (strcmp(line, "F\n") == 0) {
            // std::cout << "**End of Batch**" << '\n';
            // free(line);
            continue;
        }

        initializeStats();
        QueryInfo * queryInfo = parseInput(line);
        // joinEnumeration(queryInfo);
        std::cout << '\n';

        // std::cout << "RE-ORDERED PREDICATES" << std::endl;
        // for (uint64_t i = 0; i < queryInfo->predicatesCount; i++) {
        //     std::cout << "\t";
        //     printPredicate(&queryInfo->predicates[i]);
        // }

        Intermediate * IR = new Intermediate;
        for(uint64_t i=0; i<4; i++)
            IR->results[i] = NULL;
        IR->length = 0;

        for (uint64_t i = 0; i < queryInfo->predicatesCount; i++) {
            printPredicate(&queryInfo->predicates[i]);
            execute(&queryInfo->predicates[i], queryInfo->relations, IR);
        }

        calculateSums(queryInfo, IR);

        // std::cout << "Intermediate Results after query execution:" << '\n';
        // printResult(IR->results, IR->relCount);

        deleteIntermediate(IR);

        deleteQueryInfo(queryInfo);
        // free(line);
        // line = NULL;
    }
    free(line);
}

int main(void){
    mapAllData(&r, &relationsSize);
    createStats();

    if( r == NULL ){ //no file found
        std::cout << "No file found. Please try again. Bye!" << '\n';
        return -1;
    }

    // Print the data from a given file
    // if(relationsSize) printData(r[0]);
    std::cout << '\n';

    myJobScheduler = new JobScheduler();
    myJobScheduler->Init(4);

    //execute queries etc
    executeQueries();

    // initializeStats();

    // Stats newStats = evalJoinStats(0,2,1,0);
    // std::cout << "Evaluated = " << newStats.f << '\n';


    // std::cout << "\n\nBefore\n";
    // printStats(0);
    // std::cout << '\n';
    // printStats(1);
    // std::cout << '\n';
    // std::cout << '\n';
    // std::cout << "0 JOIN 1" << '\n';
    // updateJoinStats(0,2,1,0);
    // updateGreaterFilterStats(0,1,2500);
    // std::cout << "\n\nAfter\n";
    // printStats(0);
    // std::cout << '\n';
    // printStats(1);


    // if (stats[1][0].f == 0) {
    //     std::cout << "uwu" << '\n';
    // }

    deleteStats();
    unMapAllData(r, relationsSize);

    myJobScheduler->Stop();
    myJobScheduler->Destroy();
    delete myJobScheduler;

    return 0;
}
