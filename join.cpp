#include "join.hpp"

extern int numberOfBuckets;

void join(relation * A, relation * B){
    // Order given touples bucket by bucket (basically produces A' and B')
    uint32_t * histogramA;
    uint32_t * startingPosA;
    int * bucket_array;
    int * chain_array;
    tuple * orderedA = bucketify(A,&histogramA,&startingPosA);

    uint32_t * histogramB;
    uint32_t * startingPosB;
    tuple * orderedB = bucketify(B,&histogramB,&startingPosB);

    // Print out the hashed values of A' and B'
    // to confirm that they are in order
    // std::cout << std::endl << "A\'\t|\tB\'" << std::endl;
    // for(uint32_t i=0; i<A->size; i++){
    //     std::cout << h1(orderedA[i].value) << "\t|\t";
    //     std::cout << h1(orderedB[i].value) << std::endl;
    // }

    std::cout << "histogramA[0] = " << histogramA[0] << '\n';
    std::cout << "startingPosA[0] = " << startingPosA[0] << '\n';
    bucketify2(A, histogramA[0], startingPosA[0], &bucket_array, &chain_array);

    deleteRelation(A);
    deleteRelation(B);

    delete[] orderedA;
    delete[] histogramA;
    delete[] startingPosA;

    delete[] orderedB;
    delete[] histogramB;
    delete[] startingPosB;
}
