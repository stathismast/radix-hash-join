#include "join/memmap.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CHECK_OR_EXIT(value)                                        \
{                                                                   \
    if (value == NULL) {                                            \
        std::cout << "Error at memory allocation." << std::endl;    \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
}

struct Node{
    Node * next;
    char* filePath;
};

struct List{
    Node * first;
    Node * last;
    uint64_t count;
};

struct FileArray{
    uint64_t size;
    char ** filePaths;
};

Node * newNode(char* filePath){
    Node * temp;

    temp = new Node;
    CHECK_OR_EXIT(temp);

    temp->next = NULL;
    temp->filePath = filePath;

    return temp;
}

List * newList(char* filePath){
    List * list;

    list = new List;
    CHECK_OR_EXIT(list);

    list->first = newNode(filePath);
    list->last = list->first;
    list->count = 1;

    return list;
}

void deleteList(List * list){
    Node * current;
    Node * temp;

    if(list == NULL) return;

    current = list->first;
    while(current != NULL){
        temp = current;
        current = current->next;
        delete temp;
    }
    delete list;
}

void insertFile(List * list, char* filePath){
    Node * temp;
    temp = newNode(filePath);
    list->last->next = temp;
    list->last = temp;
    list->count ++;
}

void printFiles(List * list){
    Node * current;
    current = list->first;
    while(current != NULL){
        std::cout << current->filePath << '\n';
        current = current->next;
    }
}

FileArray * convertToArray(List * list){
    FileArray *array;

    if(list == NULL) return NULL;

    array = new FileArray;
    CHECK_OR_EXIT(array);
    array->size = list->count;
    array->filePaths = new char * [list->count];
    CHECK_OR_EXIT(array->filePaths);

    Node * current = list->first;
    uint64_t i = 0;
    while(current != NULL){
        array->filePaths[i] = current->filePath;
        current = current->next;
        i++;
    }

    return array;
}

//Only here we delete the filePaths "instances"
void deleteArray(FileArray * array){
    if(array == NULL) return;
    if( array->filePaths != NULL ){
        for( uint64_t i = 0; i < array->size; i ++ ){
            if( array->filePaths[i] != NULL ){
                delete[] array->filePaths[i];
            }
        }
        delete[] array->filePaths;
    }
    delete array;
}

//TO DO
//we can pass FILE* if we want to read from file instead of stdin
FileArray * readFileCatalog(){
    char * line = NULL;
    size_t s = 0;
    char * tempFile;
    List * tempList;
    FileArray * tempArray;

    //first filePath
    if (getline(&line, &s, stdin)<=0){
        return NULL;
    }
    //not strlen+1 as we dont need the '\n'
    tempFile = new char[strlen(line)];
    //copy filePath except '\n'
    memcpy(tempFile,line,strlen(line)-1);
    tempFile[strlen(line)-1] = '\0';
    tempList = newList(tempFile);

    //rest file paths
    while(getline(&line, &s, stdin) > 0){
        // std::cout << "Read: " << line << '\n';
        // std::cout << "strlen(line): " << strlen(line) << '\n';
        tempFile = new char[strlen(line)];

        //copy filePath except '\n'
        memcpy(tempFile,line,strlen(line)-1);
        tempFile[strlen(line)-1] = '\0';
        insertFile(tempList,tempFile);
    }

    free(line);

    //convertToArray
    // std::cout << "Print List filePaths:" << '\n';
    // printFiles(tempList);

    tempArray = convertToArray(tempList);

    // std::cout << "Print Array filePaths:" << '\n';
    // for ( uint64_t i = 0; i < tempArray->size; i ++ ){
    //     std::cout << tempArray->filePaths[i] << '\n';
    // }

    deleteList(tempList);
    return tempArray;
}

const char ** hardcodedInputFiles(int * fileCount){
    *fileCount = 14;

    const char ** inputFiles = new const char*[*fileCount];

    inputFiles[0] = "workload/r0";
    inputFiles[1] = "workload/r1";
    inputFiles[2] = "workload/r2";
    inputFiles[3] = "workload/r3";
    inputFiles[4] = "workload/r4";
    inputFiles[5] = "workload/r5";
    inputFiles[6] = "workload/r6";
    inputFiles[7] = "workload/r7";
    inputFiles[8] = "workload/r8";
    inputFiles[9] = "workload/r9";
    inputFiles[10] = "workload/r10";
    inputFiles[11] = "workload/r11";
    inputFiles[12] = "workload/r12";
    inputFiles[13] = "workload/r13";

    return inputFiles;
}

//global
Relation * r;
uint64_t relationsSize;

void Init(){
    FileArray * inputFiles;

    inputFiles = readFileCatalog();

    relationsSize = inputFiles->size;

    // Allocate an array of relations according to the number of input files
    r = new Relation[relationsSize];


    // Map every file to memory
    for( uint64_t i = 0; i < relationsSize; i++ ){
        r[i] = mapFile(inputFiles->filePaths[i]);
    }

    // Print the data from a given file
    printData(r[0]);

    deleteArray(inputFiles);
}

void Destroy(){
    // Unmap data and delete 2D index
    for(uint64_t i=0; i<relationsSize; i++){
        unmapData(r[i]);
    }
    delete[] r;
}

int main(void){
    // int fileCount;
    // const char ** inputFiles = hardcodedInputFiles(&fileCount);
    // FileArray * inputFiles;
    //
    // inputFiles = readFileCatalog();
    //
    // // Allocate an array of relations accoring to the number of input files
    // Relation * r = new Relation[inputFiles->size];
    //
    // // Map every file to memory
    // for(uint64_t i=0; i<inputFiles->size; i++){
    //     r[i] = mapFile(inputFiles->filePaths[i]);
    // }
    //
    // // Print the data from a given file
    // printData(r[0]);
    //
    // // Unmap data and delete 2D index
    // for(uint64_t i=0; i<inputFiles->size; i++){
    //     unmapData(r[i]);
    // }
    //
    // //delete[] inputFiles;
    //
    // delete[] r;

    Init();

    //execute queries etc

    Destroy();

    return 0;
}
