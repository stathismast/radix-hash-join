#include "stringList.hpp"

SLNode * newSLNode(char* filePath){
    SLNode * temp;

    temp = new SLNode;
    CHECK_OR_EXIT(temp);

    temp->next = NULL;
    temp->filePath = filePath;

    return temp;
}

StringList * newStringList(char* filePath){
    StringList * list;

    list = new StringList;
    CHECK_OR_EXIT(list);

    list->first = newSLNode(filePath);
    list->last = list->first;
    list->count = 1;

    return list;
}

void deleteStringList(StringList * list){
    SLNode * current;
    SLNode * temp;

    if(list == NULL) return;

    current = list->first;
    while(current != NULL){
        temp = current;
        current = current->next;
        delete temp;
    }
    delete list;
}

void insertFile(StringList * list, char* filePath){
    SLNode * temp;
    temp = newSLNode(filePath);
    list->last->next = temp;
    list->last = temp;
    list->count ++;
}

void printFiles(StringList * list){
    SLNode * current;
    current = list->first;
    while(current != NULL){
        std::cout << current->filePath << '\n';
        current = current->next;
    }
}

char ** convertToArray(StringList * list){
    char ** array;

    if(list == NULL) return NULL;

    array = new char * [list->count];
    CHECK_OR_EXIT(array);

    SLNode * current = list->first;
    uint64_t i = 0;
    while(current != NULL){
        array[i] = current->filePath;
        current = current->next;
        i++;
    }

    return array;
}
