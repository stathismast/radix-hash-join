#include <iostream>
#include "h1.hpp"

using namespace std;

int main(){
    srand(time(NULL));

    // Create two random columns that will be joined
    relation * A = randomRelation(50);
    relation * B = randomRelation(50);

    // Calculate histograms
    uint32_t * histogramA = calculateHistogram(A);
    uint32_t * histogramB = calculateHistogram(B);

    // Print generated histograms
    cout << "Histogram for A:" << endl;
    printHistogram(histogramA);
    cout << "Histogram for B:" << endl;
    printHistogram(histogramB);

    // Calculate starting position of each bucket in A and B
    uint32_t * startingPosA = calculateStartingPositions(histogramA);
    uint32_t * startingPosB = calculateStartingPositions(histogramB);

    // Print generated starting position arrays
    cout << "Starting positions for buckets in A:" << endl;
    printHistogram(startingPosA);
    cout << "Starting positions for buckets in B:" << endl;
    printHistogram(startingPosB);

    // Order given touples bucket by bucket (basically produces A' and B')
    tuple * orderedA = order(A,startingPosA);
    tuple * orderedB = order(B,startingPosB);

    // Print out the hashed value of A' and B'
    // to confirm that they are in order
    cout << endl << "A\'\t|\tB\'" << endl;
    for(uint32_t i=0; i<A->size; i++){
        cout << h1(orderedA[i].value) << "\t|\t";
        cout << h1(orderedB[i].value) << endl;
    }

    // Freeing allocated memory is not necessary since this is just a test
}
