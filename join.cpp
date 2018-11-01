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

    // bucketify2(A, histogramA[0], startingPosA[0], &bucket_array, &chain_array);
    for (int i = 0; i < numberOfBuckets; i++) {
        // std::cout << "Bucket " << i << std::endl << std::endl;
        // make the second hash to the smallest bucket
        if (histogramA[i] < histogramB[i]) {
            bucketify2(A, histogramA[i], startingPosA[i], &bucket_array, &chain_array);
            // compare();
        }
        else {
            bucketify2(A, histogramB[i], startingPosB[i], &bucket_array, &chain_array);
        }

        // Compare goes here

        delete[] bucket_array;
        delete[] chain_array;
        std::cout << "----------------------------------------------------------" << '\n';
    }

    deleteRelation(A);
    deleteRelation(B);

    delete[] orderedA;
    delete[] histogramA;
    delete[] startingPosA;

    delete[] orderedB;
    delete[] histogramB;
    delete[] startingPosB;

}


/*Relation A will contain the bigger array*/
void compare(relation * resA,
            relation * resB,
            uint32_t * histogramA,
            uint32_t * startingPosA,
            uint32_t * histogramB,
            uint32_t * startingPosB,
            int * bucket_array,
            int* chain_array,
            int bucket_num) {

    // sizeA =
    uint32_t i;
    int hash_value, prime = nextPrime(histogramB[bucket_num]);
    for (i = startingPosA[bucket_num]; i < histogramA[bucket_num]; i++) {
        hash_value = h2(resA->column[i].value, prime);

    }





}
