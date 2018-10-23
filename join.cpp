#include "join.hpp"

void join(relation * A, relation * B){
    // Order given touples bucket by bucket (basically produces A' and B')
    uint32_t * histogramA;
    uint32_t * startingPosA;
    tuple * orderedA = bucketify(A,&histogramA,&startingPosA);

    uint32_t * histogramB;
    uint32_t * startingPosB;
    tuple * orderedB = bucketify(B,&histogramB,&startingPosB);

    // Print out the hashed values of A' and B'
    // to confirm that they are in order
    std::cout << std::endl << "A\'\t|\tB\'" << std::endl;
    for(uint32_t i=0; i<A->size; i++){
        std::cout << h1(orderedA[i].value) << "\t|\t";
        std::cout << h1(orderedB[i].value) << std::endl;
    }

    // Code for the second part of the joining process should be here

    deleteRelation(A);
    deleteRelation(B);

    delete[] orderedA;
    delete[] histogramA;
    delete[] startingPosA;

    delete[] orderedB;
    delete[] histogramB;
    delete[] startingPosB;
}
