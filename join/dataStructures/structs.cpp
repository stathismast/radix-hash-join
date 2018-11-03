#include "structs.hpp"

// Create and return a tuple array with random values
// Max value of random number is 9999
tuple * randomColumn(uint32_t size){
    tuple * array = new tuple[size];
    for(uint32_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = rand()%10;
    }
    return array;
}

// Create and return a relation with random values
relation * randomRelation(uint32_t size){
    relation * rel = new relation;
    rel->column = randomColumn(size);
    rel->size = size;
    return rel;
}

void deleteRelation(relation * rel){
    delete[] rel->column;
    delete rel;
}

void printRelation(relation * rel){
    for(uint32_t i=0; i<rel->size; i++){
        std::cout << rel->column[i].rowid << ". "
             << rel->column[i].value << std::endl;
    }
    std::cout << std::endl;
}
