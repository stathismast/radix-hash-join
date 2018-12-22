#include "jobs.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {

    uint64_t * arrayA = new uint64_t[20];

    for(uint64_t i = 0; i<20; i++){
        arrayA[i] = rand()%5;
    }

    std::cout << "arrayA:" << '\n';
    for(uint64_t i = 0; i<20; i++){
        std::cout << arrayA[i] << '\n';
    }

    HistogramJob* jobsArray[4];

    uint64_t * startA = arrayA;

    //jobs for histogramA
    for (uint64_t i = 0; i < 4; i++) {
        jobsArray[i] = new HistogramJob(startA,5);
        startA += 5;
    }

    for (int i = 0; i < 4; i++) {
        jobsArray[i]->Run();
        std::cout << '\n';
    }

    uint64_t * myHistogramA = calculateThreadHistogram(arrayA,20);

    std::cout << "Main histogram:" << '\n';
    printHistogram(myHistogramA);

    //construct the whole histogramA
    uint64_t * wholeHistogram = new uint64_t[numberOfBuckets];
    for(uint64_t i=0; i<numberOfBuckets; i++){
        wholeHistogram[i] = 0;
        for (int j = 0; j < 4; j++) {
            wholeHistogram[i] += jobsArray[j]->myHistogram[i];
        }
    }

    std::cout << "Jobs' histogram:" << '\n';
    printHistogram(wholeHistogram);

    for (int i = 0; i < 4; i++) {
        delete jobsArray[i];
        std::cout << '\n';
    }

    delete[] myHistogramA;

    delete[] wholeHistogram;

    delete[] arrayA;

    return 0;
}
