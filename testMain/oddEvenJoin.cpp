#include <iostream>
#include "../join/join.hpp"

using namespace std;


#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    // Relation * A = randomRelation(10000);
    // Relation * B = randomRelation(5000);

    Relation * A = oddRelation(10000);
    Relation * B = evenRelation(10000);

    std::ofstream file("Logs/arrayA.txt");
    for(uint32_t i=0; i<A->size; i++){
        file << A->column[i].rowid << ". "
             << A->column[i].value <<
             " - " << h1(A->column[i].value)  << std::endl;
    }

    file.close();

    std::ofstream file2("Logs/arrayB.txt");
    for(uint32_t i=0; i<B->size; i++){
        file2 << B->column[i].rowid << ". "
             << B->column[i].value <<
             " - " << h1(B->column[i].value)  << std::endl;
    }

    file2.close();

    clock_t cl;     //initializing a clock type
    cl = clock();   //starting time of clock

    Result * result = join(A,B);

    cl = clock() - cl;  //end point of clock

    cout << "time for (10000-10000): "<< cl/(double)CLOCKS_PER_SEC << endl;  //prints the determined ticks per second (seconds passed)

    clock_t cl2;     //initializing a clock type
    cl2 = clock();   //starting time of clock

    int total = naiveJoin(A, B);

    cl2 = clock() - cl2;  //end point of clock
    cout << "Naive join time:"<< cl2/(double)CLOCKS_PER_SEC << endl;  //prints the determined ticks per second (seconds passed)

    std::cout << "Total equals = " << total << '\n';
    std::cout << "Result entries = " << result->totalEntries << '\n';

    std::cout << "Total == Result entries: " << ((uint32_t)total == result->totalEntries)  << '\n';
    deleteResult(result);
    deleteRelation(A);
    deleteRelation(B);



    // test_nextPrime(200);

    // cout << h2(50,100) << endl;
    // cout << h2(24,13) << endl;
}
