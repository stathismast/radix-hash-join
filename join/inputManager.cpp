#include "inputManager.hpp"

// Keep the contents of given line, excluding '\n'
char * getFilePath(char * line){
    char * filePath = new char[strlen(line)];
    memcpy(filePath,line,strlen(line)-1);
    filePath[strlen(line)-1] = '\0';
    return filePath;
}

bool fileExists(char * filePath){
    return ( access( filePath, F_OK ) != -1 );
    //or
    //struct stat buffer;
    //return (stat (filePath, &buffer) == 0);
    //https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
}

bool ignoreLine(char * line){

    for( uint64_t i = 0; i < strlen(line); i ++ ){
        if( !isspace(line[i])){
            return false;
        }
    }

    return true;
}

//TO DO
//we can pass FILE* if we want to read from file instead of stdin
char ** getInputFiles(uint64_t * size){
    char * line = NULL;
    size_t s = 0;
    char * filePath;
    StringList * filePathList = NULL;
    char ** inputFiles;

    filePathList = newStringList();

    while(getline(&line, &s, stdin) > 0){

        //std::cerr << "line = " << line << '\n';
        if(strcmp(line, "Done\n") == 0) {
            break;
        }

        if(ignoreLine(line)){
            continue;
        }

        filePath = getFilePath(line);
        if(!fileExists(filePath)){
            std::cerr << "File " << filePath << " doesn't exist.\n\n";
            delete[] filePath;
            continue;
        }

        insertFile(filePathList,filePath);

    }

    if( isEmpty(filePathList) ){ //no file found
        free(line);
        deleteStringList(filePathList);
        return NULL;
    }

    free(line);

    inputFiles = convertToArray(filePathList);

    *size = filePathList->count;
    deleteStringList(filePathList);

    return inputFiles;
}
