#include "join.hpp"

extern uint32_t numberOfBuckets;

Result * join(relation * A, relation * B){
    // Order given touples bucket by bucket (basically produces A' and B')
    uint32_t * histogramA;
    uint32_t * startingPosA;
    tuple * orderedA = bucketify(A,&histogramA,&startingPosA);

    uint32_t * histogramB;
    uint32_t * startingPosB;
    tuple * orderedB = bucketify(B,&histogramB,&startingPosB);

    std::cout << "Original A array:" << std::endl;
    printRelation(A);
    std::cout << "Original B array:" << std::endl;
    printRelation(B);

    // Print out the hashed values of A' and B'
    // to confirm that they are in order
    // std::cout << std::endl << "index\tA\' Bu\t|\tB\' Bu" << std::endl;
    // for(uint32_t i=0; i<A->size; i++){
    //     std::cout << h1(orderedA[i].value) << "\t|\t";
    //     std::cout << h1(orderedB[i].value) << std::endl;
    //     std::cout << i << "\t" << h1(orderedA[i].value) << "  " << orderedA[i].value << "\t|\t";
    //     std::cout << "\t" << h1(orderedB[i].value) << "  " << orderedB[i].value <<std::endl;
    // }

    uint32_t * bucketArray;
    uint32_t * chainArray;
    Result * result = newResult();
    for (uint32_t i = 0; i < numberOfBuckets; i++) {

        if(histogramA[i] == 0 || histogramB[i] == 0){
            //the one bucket is empty so there is nothing to compare with
            //the other bucket
            continue;
        }
        // For each bucket find the smaller one and make an index with h2 for
        // that one. Then find the equal values and store them in result
        if (histogramA[i] >= histogramB[i]) {
            bucketify2(orderedB, histogramB[i], startingPosB[i], &bucketArray, &chainArray);
            compare(orderedA, orderedB, histogramA[i], startingPosA[i], histogramB[i], \
                startingPosB[i], bucketArray, chainArray, result, 0);
        }
        else {
            bucketify2(orderedA, histogramA[i], startingPosA[i], &bucketArray, &chainArray);
            compare(orderedB, orderedA, histogramB[i], startingPosB[i], histogramA[i], \
                startingPosA[i], bucketArray, chainArray, result, 1);
        }

        delete[] bucketArray;
        delete[] chainArray;
    }

    printResult(result);

    delete[] orderedA;
    delete[] histogramA;
    delete[] startingPosA;

    delete[] orderedB;
    delete[] histogramB;
    delete[] startingPosB;

    return result;
}

void compare(tuple * orderedBig,
            tuple * orderedSmall,
            uint32_t bucketSizeBig,
            uint32_t startIndexBig,
            uint32_t bucketSizeSmall,
            uint32_t startIndexSmall,
            uint32_t * bucketArray,
            uint32_t * chainArray,
            Result * result,
            char flag) {

    int hash_value, prime = nextPrime(bucketSizeSmall);
    // Compare every value of the bigger relation with the values of the smaller
    // one but are on the same bucket of h2
    for (uint32_t i = startIndexBig;
                  i < bucketSizeBig + startIndexBig;
                  i++) {
        hash_value = h2(orderedBig[i].value, prime);
        checkEquals(&orderedBig[i], hash_value, orderedSmall, bucketSizeSmall, startIndexSmall, \
            bucketArray, chainArray, result, flag, i);
    }
}

void checkEquals(tuple * tupleA,
                int hash_value,
                tuple * orderedSmall,
                uint32_t bucketSizeSmall,
                uint32_t startIndexSmall,
                uint32_t * bucketArray,
                uint32_t * chainArray,
                Result * result,
                char flag,
                int rowIdBig) {
    tuple * tupleB;
    // Get the rowId of the first value in the current bucket
    int rowIdSmall = bucketArray[hash_value], normRowId;
    while (rowIdSmall != -1) {
        // We need to normalse the rowid because it has values from -1 to
        // the size of the bucket but we are using the actual array
        normRowId = rowIdSmall + startIndexSmall;
        // Get the value from the bigger array
        tupleB = &orderedSmall[normRowId];
        // Compare the values and add them in the list if they are wqual
        if (tupleA->value == tupleB->value) {
            // We need this flag so we will know which rowId goes first and
            // which second
            if (flag == 0) {
                insertResult(result,tupleA->rowid, tupleB->rowid);
            }
            else{
                insertResult(result,tupleB->rowid, tupleA->rowid);
            }
        }
        // Find the next element in the current bucket
        rowIdSmall = chainArray[rowIdSmall];
    }

}

// Returns the number of matches between the two given relations
int naiveJoin(relation * A, relation * B) {
    int counter = 0;
    for(uint32_t i=0; i<A->size; i++){
        for(uint32_t j=0; j<B->size; j++){
            if(A->column[i].value == B->column[j].value){
                counter++;
            }
        }
    }
    return counter;
}
