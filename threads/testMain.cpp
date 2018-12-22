#include "jobs.hpp"
#include <iostream>

extern uint64_t * psums[4];
extern uint64_t * histograms[4];

int main(int argc, char const *argv[]) {

    uint64_t * arrayA = new uint64_t[20];

    for(uint64_t i = 0; i<20; i++){
        arrayA[i] = rand()%5;
    }

    std::cout << "arrayA:" << '\n';
    for(uint64_t i = 0; i<20; i++){
        std::cout << arrayA[i] << '\n';
    }

    // Calculate histograms
    HistogramJob* jobsArray[4];
    uint64_t * startA = arrayA;

    //jobs for histogramA
    uint64_t length = 20 / 4;
    uint64_t lastExtra = 20 % 4;

    uint64_t i;
    for (i = 0; i < 3; i++) {
        jobsArray[i] = new HistogramJob(startA,length,&histograms[i]);
        startA += length;
    }
    //last thread may take extra length
    jobsArray[i] = new HistogramJob(startA,length+lastExtra,&histograms[i]);

    for (uint64_t i = 0; i < 4; i++) {
        jobsArray[i]->Run();
    }

    uint64_t * myHistogramA = calculateThreadHistogram(arrayA,20);

    std::cout << "Main histogram:" << '\n';
    printHistogram(myHistogramA);

    //construct the whole histogramA
    uint64_t * wholeHistogram = new uint64_t[numberOfBuckets];
    for(uint64_t i=0; i<numberOfBuckets; i++){
        wholeHistogram[i] = 0;
        for (int j = 0; j < 4; j++) {
            wholeHistogram[i] += histograms[j][i];
        }
    }

    for (int j = 0; j < 4; j++) {
        std::cout << "Histogram of Job " << j << ":" << '\n';
        printHistogram(histograms[j]);
    }

    //calculate psums for each next thread
    for (uint64_t i = 0; i < 4; i++) {
        psums[i] = new uint64_t[numberOfBuckets];
    }

    //for first psum
    psums[0][0] = 0;
    for (uint64_t i = 1; i < 4; i++) {
        psums[i][0] = psums[i-1][0] + histograms[i-1][0];
    }

    //for rest psums
    for(uint64_t i=1; i<numberOfBuckets; i++){
        psums[0][i] = psums[3][i-1] + histograms[3][i-1];
        for(uint64_t j = 1; j < 4; j++){
            psums[j][i] = psums[j-1][i] + histograms[j-1][i];
        }
    }

    for (uint64_t i = 0; i < 4; i++) {
        std::cout << "psum of job " << i << " is:" << '\n';
        for(uint64_t j=0; j<numberOfBuckets; j++){
            std::cout << psums[i][j] << '\n';
        }
        std::cout << '\n';
    }


    std::cout << "Jobs' histogram:" << '\n';
    printHistogram(wholeHistogram);

    for (int i = 0; i < 4; i++) {
        delete jobsArray[i];
        delete[] histograms[i];
        delete[] psums[i];
    }

    delete[] myHistogramA;

    delete[] wholeHistogram;

    delete[] arrayA;

    return 0;
}
