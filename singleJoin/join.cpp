#include "join.hpp"

extern uint64_t numberOfBuckets;

uint64_t * globalHistA;
uint64_t * globalPsumA;
uint64_t * globalHistB;
uint64_t * globalPsumB;

Result *** globalResults;

Column * orderedA;
Column * orderedB;

#define USE_THREADS 0

Result ** join(Column * A, Column * B){
    if(USE_THREADS){
        orderedA = bucketifyThread(A, &globalHistA, &globalPsumA);
        orderedB = bucketifyThread(B, &globalHistB, &globalPsumB);

        globalResults = new Result**[numberOfBuckets];
        threadJoin(numberOfBuckets);
        Result ** threadResult = convertResult(numberOfBuckets);

        for(uint64_t i=0; i<numberOfBuckets; i++){
            deleteResult(globalResults[i][0]);
            deleteResult(globalResults[i][1]);
            delete[] globalResults[i];
        }
        delete[] globalResults;

        deleteColumn(orderedA);
        delete[] globalHistA;
        delete[] globalPsumA;

        deleteColumn(orderedB);
        delete[] globalHistB;
        delete[] globalPsumB;

        return threadResult;
    }
    else{
        // Order given touples bucket by bucket (basically produces A' and B')
        uint64_t * histogramA;
        uint64_t * startingPosA;
        Column * orderedA = bucketify(A, &histogramA, &startingPosA);

        uint64_t * histogramB;
        uint64_t * startingPosB;
        Column * orderedB = bucketify(B, &histogramB, &startingPosB);

        uint64_t * bucketArray;
        uint64_t * chainArray;
        Result ** result = new Result*[2];
        result[0] = newResult();
        result[1] = newResult();
        for (uint64_t i = 0; i < numberOfBuckets; i++) {
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

        //printDoubleResult(result);

        deleteColumn(orderedA);
        delete[] histogramA;
        delete[] startingPosA;

        deleteColumn(orderedB);
        delete[] histogramB;
        delete[] startingPosB;

        return result;
    }
}

void compare(Column * orderedBig,
            Column * orderedSmall,
            uint64_t bucketSizeBig,
            uint64_t startIndexBig,
            uint64_t bucketSizeSmall,
            uint64_t startIndexSmall,
            uint64_t * bucketArray,
            uint64_t * chainArray,
            Result ** result,
            bool flag) {

    int hash_value, prime = nextPrime(bucketSizeSmall);
    // Compare every value of the bigger Column with the values of the smaller
    // one but are on the same bucket of h2
    for (uint64_t i = startIndexBig;
                  i < bucketSizeBig + startIndexBig;
                  i++) {
        hash_value = h2(orderedBig->value[i], prime);
        checkEquals(orderedBig->rowid[i], orderedBig->value[i], hash_value, \
                    orderedSmall, startIndexSmall, bucketArray, chainArray, \
                    result, flag, i);
    }
}

void checkEquals(uint64_t rowidA,
                 uint64_t valueA,
                 int hash_value,
                 Column * orderedSmall,
                 uint64_t startIndexSmall,
                 uint64_t * bucketArray,
                 uint64_t * chainArray,
                 Result ** result,
                 bool flag,
                 int rowIdBig) {

    uint64_t rowidB;
    uint64_t valueB;
    // Get the rowId of the first value in the current bucket
    int rowIdSmall = bucketArray[hash_value], normRowId;
    while (rowIdSmall != -1) {
        // We need to normalse the rowid because it has values from -1 to
        // the size of the bucket but we are using the actual array
        normRowId = rowIdSmall + startIndexSmall;
        // Get the value from the bigger array
        rowidB = orderedSmall->rowid[normRowId];
        valueB = orderedSmall->value[normRowId];
        // Compare the values and add them in the list if they are wqual
        if (valueA == valueB) {
            // We need this flag so we will know which rowId goes first and
            // which second
            if (flag == 0) {
                insertSingleResult(result[0], rowidA);
                insertSingleResult(result[1], rowidB);
            }
            else{
                insertSingleResult(result[0], rowidB);
                insertSingleResult(result[1], rowidA);
            }
        }
        // Find the next element in the current bucket
        rowIdSmall = chainArray[rowIdSmall];
    }

}

// Returns the number of matches between the two given Columns
int naiveJoin(Column * A, Column * B) {
    int counter = 0;
    for(uint64_t i=0; i<A->size; i++){
        for(uint64_t j=0; j<B->size; j++){
            if(A->value[i] == B->value[j]){
                counter++;
            }
        }
    }
    return counter;
}
