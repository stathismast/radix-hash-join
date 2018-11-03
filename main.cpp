#include <iostream>
#include "./join/join.hpp"

using namespace std;

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    relation * A = segFaultTupleA();
    relation * B = segFaultTupleB();

    Result * result = join(A,B);

    std::cout << "Total equals = " << naiveJoin(A, B) << '\n';
    std::cout << "Result entries = " << result->totalEntries << '\n';

    deleteResult(result);
    deleteRelation(A);
    deleteRelation(B);



    // test_nextPrime(200);

    // cout << h2(50,100) << endl;
    // cout << h2(24,13) << endl;
}
