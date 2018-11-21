#include "join/memmap.hpp"
#include "join/stringList.hpp"

// Keep the contents of given line, excluding '\n'
char * getFilePath(char * line){
    char * filePath = new char[strlen(line)];
    memcpy(filePath,line,strlen(line)-1);
    filePath[strlen(line)-1] = '\0';
    return filePath;
}

//TO DO
//we can pass FILE* if we want to read from file instead of stdin
char ** getInputFiles(uint64_t * size){
    char * line = NULL;
    size_t s = 0;
    char * filePath;
    StringList * filePathList;
    char ** inputFiles;

    // First file path
    if (getline(&line, &s, stdin) <= 0){
        return NULL;
    }
    filePath = getFilePath(line);
    filePathList = newStringList(filePath);

    // Remaining file paths
    while(getline(&line, &s, stdin) > 0){
        filePath = getFilePath(line);
        insertFile(filePathList,filePath);
    }

    free(line);

    inputFiles = convertToArray(filePathList);

    *size = filePathList->count;
    deleteStringList(filePathList);

    return inputFiles;
}

//global
Relation * r;
uint64_t relationsSize;

void mapAllData(){
    char ** inputFiles = getInputFiles(&relationsSize);

    // Allocate an array of relations according to the number of input files
    r = new Relation[relationsSize];

    // Map every file to memory
    for( uint64_t i = 0; i < relationsSize; i++ ){
        r[i] = mapFile(inputFiles[i]);
    }

    // Deallocate memory used for file paths
    for(uint64_t i=0; i<relationsSize; i++){
        std::cout << inputFiles[i] << std::endl;
        delete[] inputFiles[i];
    }
    delete[] inputFiles;
}

void unMapAllData(){
    // Unmap data and delete 2D index
    for(uint64_t i=0; i<relationsSize; i++){
        unmapData(r[i]);
    }
    delete[] r;
}

int main(void){

    mapAllData();

    // Print the data from a given file
    if(relationsSize) printData(r[0]);

    //execute queries etc

    unMapAllData();

    return 0;
}
