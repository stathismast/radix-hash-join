#include <iostream>
#include "h1.hpp"

using namespace std;

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    relation * A = randomRelation(50);
    relation * B = randomRelation(50);

    // Order given touples bucket by bucket (basically produces A' and B')
    tuple * orderedA = bucketify(A);
    tuple * orderedB = bucketify(B);

    // Print out the hashed values of A' and B'
    // to confirm that they are in order
    cout << endl << "A\'\t|\tB\'" << endl;
    for(uint32_t i=0; i<A->size; i++){
        cout << h1(orderedA[i].value) << "\t|\t";
        cout << h1(orderedB[i].value) << endl;
    }

    deleteRelation(A);
    deleteRelation(B);
    delete[] orderedA;
    delete[] orderedB;
}
