/***************************************************************************************
Source file : init.cpp
Description : Implementation of functions which are used to map the relations
              from disk to memory.
****************************************************************************************/

#include "init.hpp"

//global mmap with size numOfRelations
uint64_t ** globalData;
uint64_t globalDataSize;

RelationMap * init(){

    //read from stdin and store in a list so as to know the number
    //of all relations -> globalDataSize


    char testFile1[] = "./testMap/fileCatalog/r0";
    char testFile2[] = "./testMap/fileCatalog/r1";

    globalDataSize = 1;

    //init globalData
    globalData = new uint64_t*[globalDataSize];

    for( uint64_t i = 0; i < globalDataSize; i ++ ){
        globalData[i] = NULL;
    }

    //init RelationMap
    RelationMap * relMap = newRelationMap(globalDataSize);

    //for each relFile do the following
    Relation * temp = initRelation(testFile1,0);

    insertRelation(relMap,0,temp);

    return relMap;
}

Relation* initRelation(char * relFile,uint64_t index){

    //we need buffer for only 2 values (numRows and numCols)
    char buffer[HEADER_SIZE];
    uint64_t numRows,numCols;
    Relation * rel;
    std::ifstream myFile (relFile, std::ios::in | std::ios::binary);

    myFile.read (buffer, HEADER_SIZE);
    // if (!myFile) {
    //     std::cout << "fuck!" << '\n';
    //     // An error occurred!
    //     std::cout << "lol: gcount is: " << myFile.gcount() << '\n';
    //     // myFile.gcount() returns the number of bytes read.
    //     // calling myFile.clear() will reset the stream state
    //     // so it is usable again.
    // }

    memcpy(&numRows,buffer,sizeof(uint64_t));
    memcpy(&numCols,buffer+sizeof(uint64_t),sizeof(uint64_t));

    // std::cout << "numRows: " << numRows << '\n';
    // std::cout << "numCols: " << numCols << '\n';
    // std::cout << "numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) = " <<
    // numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t) << '\n';

    //make a new relation
    rel = newRelation(numCols,numRows);

    //map columns
    uint64_t fd,size;
    off_t offset, pa_offset;
    size_t length;

    // size = numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t);
    offset = 2*sizeof(uint64_t);
    // offset = 0;
    //
    // std::cout << "offset is: " << offset << '\n';

    length = numRows*numCols*sizeof(uint64_t)+2*sizeof(uint64_t);
    std::cout << "length is: " << length << '\n';

    pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
    /* offset for mmap() must be page aligned */
    // std::cout << "pa_offset is: " << pa_offset << '\n';

    fd = open(relFile, O_RDONLY);
    if (fd == -1){
        std::cout << "Error at open!" << '\n';
    }

    globalData[index] = (uint64_t*) mmap(NULL, length ,
                PROT_READ,MAP_PRIVATE, fd, pa_offset);

    if (globalData[index] == MAP_FAILED){
        std::cout << "error at mmap!" << '\n';
    }


    // if(globalData[index] == NULL ){
    //     std::cout << "AHHAHAHA EINAI NULL!!!" << '\n';
    // }
    // else{
    //     std::cout << "DEN EINAI NULL!!!!" << '\n';
    //     std::cout << "alla to [0][0] einai " << globalData[0][0]  << '\n';
    // }

    //map each column
    uint64_t cur_start=2*sizeof(uint64_t); //skip header
    uint64_t *toSave;
    for( uint64_t i = 0; i < numCols; i ++ ){
        // std::cout << "for i:" << i <<" cur_start is : " << cur_start << '\n';
        // std::cout << "numRows is: " << numRows << '\n';
        // std::cout << "alla to [index][cur_start] einai " << "[" << index << "]"
        // <<"[" << cur_start << "] kai "
        //  << globalData[index][cur_start]  << '\n';
        toSave = &(globalData[index][cur_start]);
        setColData(rel,i,toSave);
        cur_start += numRows * sizeof(uint64_t);
    }

    close(fd);
    myFile.close();
    return rel;
}

void destroy(RelationMap * relMap){

    //for each address at globalData do munmap
    //for( uint64_t i = 0; i < globalDataSize; i ++ ){
        //we need the length + offset - pa_offset
        //of each relation
    //}

    deleteRelationMap(relMap);
    delete[] globalData;
}

void printRelation(RelationMap * relMap,uint64_t index,std::ofstream * dest){

}
