#include "inputManager.hpp"

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
        std::cout << "line = " << line << '\n';
        if (strcmp(line, "Done\n") == 0) {
            break;
        }
        filePath = getFilePath(line);
        insertFile(filePathList,filePath);
    }

    free(line);

    inputFiles = convertToArray(filePathList);

    *size = filePathList->count;
    deleteStringList(filePathList);

    return inputFiles;
}
