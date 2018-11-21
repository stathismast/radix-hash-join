

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdint.h>

#define CHECK_OR_EXIT(value)                                        \
{                                                                   \
    if (value == NULL) {                                            \
        std::cout << "Error at memory allocation." << std::endl;    \
        exit(EXIT_FAILURE);                                         \
    }                                                               \
}

typedef struct SLNode{
    SLNode * next;
    char* filePath;
} SLNode;

typedef struct StringList{
    SLNode * first;
    SLNode * last;
    uint64_t count;
} StringList;

SLNode * newSLNode(char* filePath);
StringList * newStringList(char* filePath);
void deleteStringList(StringList * list);
void insertFile(StringList * list, char* filePath);
void printFiles(StringList * list);
char ** convertToArray(StringList * list);
