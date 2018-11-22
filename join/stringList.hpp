#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdint.h>

#ifndef STRING_LIST_HPP
#define STRING_LIST_HPP

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
StringList * newStringList();
void deleteStringList(StringList * list);
void insertFile(StringList * list, char* filePath);
void printFiles(StringList * list);
char ** convertToArray(StringList * list);
bool isEmpty(StringList * list);

#endif
