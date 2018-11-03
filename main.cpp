#include <iostream>
#include "./join/join.hpp"

using namespace std;

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    relation * A = randomRelation(20);
    relation * B = randomRelation(20);

    Result * result = join(A,B);
    deleteResult(result);

    // test_nextPrime(200);

    // cout << h2(50,100) << endl;
    // cout << h2(24,13) << endl;
}
