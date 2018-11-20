#include "memmap.hpp"

int main(void){
    char inputFile[] = "../workload/r0";

    Relation r0;
    r0 = mapFile(inputFile);
    printData(r0);
    unmapData(r0);
}
