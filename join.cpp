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

    Result * result;

    // Print out the hashed values of A' and B'
    // to confirm that they are in order
    // std::cout << std::endl << "A\'\t|\tB\'" << std::endl;
    // for(uint32_t i=0; i<A->size; i++){
    //     std::cout << h1(orderedA[i].value) << "\t|\t";
    //     std::cout << h1(orderedB[i].value) << std::endl;
    // }

    // bucketify2(A, histogramA[0], startingPosA[0], &bucket_array, &chain_array);
    for (int i = 0; i < numberOfBuckets; i++) {
        // Compare goes here
        std::cout << "Bucket " << i << std::endl << std::endl;
        std::cout << "Checking for bucket " << i << '\n';

        std::cout << "Ordered bucket A:" << std::endl;
        for (uint32_t j = 0; j < histogramA[i]; j++) {
            std::cout << "\t"<< j << ": " << A->column[j].value << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Ordered bucket B:" << std::endl;
        for (uint32_t j = 0; j < histogramB[i]; j++) {
            std::cout << "\t"<< j << ": " << B->column[j].value << std::endl;
        }
        std::cout << std::endl;
        // make the second hash to the smallest bucket
        if (histogramA[i] >= histogramB[i]) {
            std::cout << "Bucket A is bigger" << '\n';
            bucketify2(B, histogramB[i], startingPosB[i], &bucket_array, &chain_array);
            compare(A, B, histogramA, startingPosA, histogramB, startingPosB, \
            bucket_array, chain_array, i, &result, 0);
        }
        else {
            std::cout << "Bucket B is bigger" << '\n';
            bucketify2(A, histogramA[i], startingPosA[i], &bucket_array, &chain_array);
            compare(B, A, histogramB, startingPosB, histogramA, startingPosA, \
            bucket_array, chain_array, i, &result, 1);
        }

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
void compare(relation * relA,
            relation * relB,
            uint32_t * histogramA,
            uint32_t * startingPosA,
            uint32_t * histogramB,
            uint32_t * startingPosB,
            int * bucket_array,
            int * chain_array,
            int bucket_num,
            Result ** result,
            char fag) {
    int rowId, normRowId;
    uint32_t i;
    int32_t valueA, valueB;
    int hash_value, prime = nextPrime(histogramB[bucket_num]);
    for (i = startingPosA[bucket_num]; i < histogramA[bucket_num] + startingPosA[bucket_num]; i++) {
        valueA = relA->column[i].value;
        hash_value = h2(valueA, prime);
        rowId = bucket_array[hash_value];
        while (rowId != -1) {
            // We need to normalse the rowid because it has values from -1 to
            // the size of the bucket. But we are using the actual array
            normRowId = rowId + startingPosB[bucket_num];
            valueB = relB->column[normRowId].value;
            if (valueA == valueB) {
                // insert to list actually
                std::cout << "Equal:" << valueA << " = " << valueB << ", " <<\
                i << ":" << rowId << '\n';
            }
            rowId = chain_array[rowId];
        }
    }
}
