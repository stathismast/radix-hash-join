// #include <fstream>
// #include <cstdio>
// #include <cstring>  // For memcpy
// #include <stdint.h>
// #include <iostream>
//
// #define HEADER_SIZE (sizeof(uint64_t) + sizeof(uint64_t))

#include "init.hpp"
int main(int argc, char const *argv[]) {

    // char buffer[HEADER_SIZE];
    //
    // char testFile1[] = "./testMap/fileCatalog/r0";
    // std::ifstream myFile (testFile1, std::ios::in | std::ios::binary);
    //
    // myFile.read (buffer, HEADER_SIZE);
    // if (!myFile) {
    //     std::cout << "fuck!" << '\n';
    //     // An error occurred!
    //     std::cout << "lol: gcount is: " << myFile.gcount() << '\n';
    //     // myFile.gcount() returns the number of bytes read.
    //     // calling myFile.clear() will reset the stream state
    //     // so it is usable again.
    // }
    //
    // uint64_t numRows,numCols;
    // memcpy(&numRows,buffer,sizeof(uint64_t));
    // memcpy(&numCols,buffer+sizeof(uint64_t),sizeof(uint64_t));
    //
    // std::cout << "numRows: " << numRows << '\n';
    // std::cout << "numCols: " << numCols << '\n';
    // std::cout << "numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) = " <<
    // numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) << '\n';
    //
    // myFile.close();
    RelationMap *relMap = init();

    destroy(relMap);

    return 0;
}
