/***************************************************************************************
Source file : init.cpp
Description : Implementation of functions which are used to map the relations
              from disk to memory.
****************************************************************************************/

#include "init.hpp"

//global mmap with size numOfRelations
uint64_t ** globalData;

RelationMap * init(){

    //read from stdin and store in a list so as to know the number
    //of all relations
    uint64_t size;

    char testFile1[] = "./testMap/fileCatalog/r0";
    char testFile2[] = "./testMap/fileCatalog/r1";

    size = 1;

    //init globalData
    globalData = new uint64_t*[size];

    //init RelationMap
    RelationMap * relMap = newRelationMap(size);

    //for each relFile do the following
    Relation * temp = initRelation(testFile1);

    insertRelation(relMap,0,temp);

    return relMap;
}

Relation* initRelation(char * relFile){

    //we need buffer for only 2 values (numRows and numCols)
    char buffer[HEADER_SIZE];
    uint64_t numRows,numCols;
    Relation * rel;
    std::ifstream myFile (relFile, std::ios::in | std::ios::binary);

    myFile.read (buffer, HEADER_SIZE);
    if (!myFile) {
        std::cout << "fuck!" << '\n';
        // An error occurred!
        std::cout << "lol: gcount is: " << myFile.gcount() << '\n';
        // myFile.gcount() returns the number of bytes read.
        // calling myFile.clear() will reset the stream state
        // so it is usable again.
    }

    memcpy(&numRows,buffer,sizeof(uint64_t));
    memcpy(&numCols,buffer+sizeof(uint64_t),sizeof(uint64_t));

    std::cout << "numRows: " << numRows << '\n';
    std::cout << "numCols: " << numCols << '\n';
    std::cout << "numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) = " <<
    numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) << '\n';

    //make a new relation
    rel = newRelation(numCols,numRows);

    //map columns

    myFile.close();
    return rel;
}

void destroy(RelationMap * relMap){
    deleteRelationMap(relMap);
    delete[] globalData;
}

void printRelation(RelationMap * relMap,uint64_t index,std::ofstream * dest){

}
