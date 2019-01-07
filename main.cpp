#include "join/parse.hpp"
#include "join/inputManager.hpp" //for ignoreLine
#include "threads/scheduler.hpp"
#include "join/stats.hpp"

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

        std::cout   << "Before 1\n"
                    << "0.0" << ": l=" << stats[0][0].l
                       << "  u=" << stats[0][0].u
                       << "  f=" << stats[0][0].f
                       << "  d=" << stats[0][0].d << "\n"
                    << "0.1" << ": l=" << stats[0][1].l
                       << "  u=" << stats[0][1].u
                       << "  f=" << stats[0][1].f
                       << "  d=" << stats[0][1].d << "\n"
                    << "0.2" << ": l=" << stats[0][2].l
                       << "  u=" << stats[0][2].u
                       << "  f=" << stats[0][2].f
                       << "  d=" << stats[0][2].d << "\n"
                     << "0.2" << ": l=" << stats[0][3].l
                        << "  u=" << stats[0][3].u
                        << "  f=" << stats[0][3].f
                        << "  d=" << stats[0][3].d << "\n";
        std::cout   << "\nBefore 1\n"
                     << "1.0" << ": l=" << stats[1][0].l
                        << "  u=" << stats[1][0].u
                        << "  f=" << stats[1][0].f
                        << "  d=" << stats[1][0].d << "\n"
                     << "1.1" << ": l=" << stats[1][1].l
                        << "  u=" << stats[1][1].u
                        << "  f=" << stats[1][1].f
                        << "  d=" << stats[1][1].d << "\n"
                     << "1.2" << ": l=" << stats[1][2].l
                        << "  u=" << stats[1][2].u
                        << "  f=" << stats[1][2].f
                        << "  d=" << stats[1][2].d << "\n";
        joinStats(0,3,1,0);
        // if (stats[1][0].f == 0) {
        //     /* code */
        //     std::cout << "uwu" << '\n';
        // }
        std::cout   << "\n\nAfter 0\n"
                    << "0.0" << ": l=" << stats[0][0].l
                       << "  u=" << stats[0][0].u
                       << "  f=" << stats[0][0].f
                       << "  d=" << stats[0][0].d << "\n"
                    << "0.1" << ": l=" << stats[0][1].l
                       << "  u=" << stats[0][1].u
                       << "  f=" << stats[0][1].f
                       << "  d=" << stats[0][1].d << "\n"
                    << "0.2" << ": l=" << stats[0][2].l
                       << "  u=" << stats[0][2].u
                       << "  f=" << stats[0][2].f
                       << "  d=" << stats[0][2].d << "\n"
                     << "0.3" << ": l=" << stats[0][3].l
                        << "  u=" << stats[0][3].u
                        << "  f=" << stats[0][3].f
                        << "  d=" << stats[0][3].d << "\n";
        std::cout   << "\nAfter 1\n"
                  << "1.0" << ": l=" << stats[1][0].l
                     << "  u=" << stats[1][0].u
                     << "  f=" << stats[1][0].f
                     << "  d=" << stats[1][0].d << "\n"
                  << "1.1" << ": l=" << stats[1][1].l
                     << "  u=" << stats[1][1].u
                     << "  f=" << stats[1][1].f
                     << "  d=" << stats[1][1].d << "\n"
                  << "1.2" << ": l=" << stats[1][2].l
                     << "  u=" << stats[1][2].u
                     << "  f=" << stats[1][2].f
                     << "  d=" << stats[1][2].d << "\n";

    deleteStats();
    unMapAllData(r, relationsSize);

    myJobScheduler->Stop();
    myJobScheduler->Destroy();
    delete myJobScheduler;

    return 0;
}
