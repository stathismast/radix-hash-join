#include "structs.hpp"

// Create and return a Tuplearray with random values
// Max value of random number is 9999
Tuple * randomColumn(uint32_t size){
    Tuple * array = new Tuple[size];
    for(uint32_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = rand()%2000;
    }
    return array;
}

// Create and return a Relation with random values
Relation * randomRelation(uint32_t size){
    Relation * rel = new Relation;
    rel->column = randomColumn(size);
    rel->size = size;
    return rel;
}

void deleteRelation(Relation * rel){
    delete[] rel->column;
    delete rel;
}

void printRelation(Relation * rel){
    for(uint32_t i=0; i<rel->size; i++){
        std::cout << rel->column[i].rowid << ". "
             << rel->column[i].value << std::endl;
    }
    std::cout << std::endl;
}
