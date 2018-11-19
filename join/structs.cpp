#include "structs.hpp"

// Create and return a Tuplearray with random values
// Max value of random number is 9999
Tuple * randomColumn(uint64_t size){
    Tuple * array = new Tuple[size];
    for(uint64_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = rand()%2000;
    }
    return array;
}

// Create and return a Relation with serial values
Relation * randomRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->column = randomColumn(size);
    rel->size = size;
    return rel;
}


// Create and return a Tuplearray with serial values
Tuple * serialColumn(uint64_t size){
    Tuple * array = new Tuple[size];
    for(uint64_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = i+1;
    }
    return array;
}


// Create and return a Relation with serial values
Relation * serialRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->column = serialColumn(size);
    rel->size = size;
    return rel;
}


// Create and return a Tuplearray with odd values
Tuple * oddColumn(uint64_t size){
    Tuple * array = new Tuple[size];
    uint64_t value = 1;
    for(uint64_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = value;
        value += 2;
    }
    return array;
}


// Create and return a Relation with odd values
Relation * oddRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->column = oddColumn(size);
    rel->size = size;
    return rel;
}


// Create and return a Tuplearray with even values
Tuple * evenColumn(uint64_t size){
    Tuple * array = new Tuple[size];
    uint64_t value = 0;
    for(uint64_t i = 0; i<size; i++){
        array[i].rowid = i+1;
        array[i].value = value;
        value += 2;
    }
    return array;
}

// Create and return a Relation with even values
Relation * evenRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->column = evenColumn(size);
    rel->size = size;
    return rel;
}


void deleteRelation(Relation * rel){
    delete[] rel->column;
    delete rel;
}

void printRelation(Relation * rel){
    for(uint64_t i=0; i<rel->size; i++){
        std::cout << rel->column[i].rowid << ". "
             << rel->column[i].value << std::endl;
    }
    std::cout << std::endl;
}
