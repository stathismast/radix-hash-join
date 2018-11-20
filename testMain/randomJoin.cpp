#include <iostream>
#include "../singleJoin/join.hpp"

using namespace std;


#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    Column * A = randomColumn(10000);
    Column * B = randomColumn(5000);

    std::ofstream file("Logs/arrayA.txt");
    for(uint64_t i=0; i<A->size; i++){
        file << A->rowid[i] << ". "
             << A->value[i] <<
             " - " << h1(A->value[i])  << std::endl;
    }

    file.close();

    std::ofstream file2("Logs/arrayB.txt");
    for(uint64_t i=0; i<B->size; i++){
        file2 << B->rowid[i] << ". "
             << B->value[i] <<
             " - " << h1(B->value[i])  << std::endl;
    }

    file2.close();

    clock_t cl;     //initializing a clock type
    cl = clock();   //starting time of clock

    Result * result = join(A,B);

    cl = clock() - cl;  //end point of clock

    cout << "time for (10000-5000): "<< cl/(double)CLOCKS_PER_SEC << endl;  //prints the determined ticks per second (seconds passed)

    clock_t cl2;     //initializing a clock type
    cl2 = clock();   //starting time of clock

    int total = naiveJoin(A, B);

    cl2 = clock() - cl2;  //end point of clock
    cout << "Naive join time:"<< cl2/(double)CLOCKS_PER_SEC << endl;  //prints the determined ticks per second (seconds passed)

    std::cout << "Total equals = " << total << '\n';
    std::cout << "Result entries = " << result->totalEntries << '\n';

    std::cout << "Total == Result entries: " << ((uint64_t)total == result->totalEntries)  << '\n';
    deleteResult(result);
    deleteColumn(A);
    deleteColumn(B);



    // test_nextPrime(200);

    // cout << h2(50,100) << endl;
    // cout << h2(24,13) << endl;
}
