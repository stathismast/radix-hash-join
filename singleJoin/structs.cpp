#include "structs.hpp"

Relation * newRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;

    // Initialize with 0 (maybe not needed)
    // for(uint64_t i = 0; i<size; i++){
    //     rel->rowid[i] = 0;
    //     rel->value[i] = 0;
    // }

    return rel;
}

// Create and return a Relation with serial values
Relation * randomRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = rand()%2000;
    }
    return rel;
}

// Create and return a Relation with serial values
Relation * serialRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = i+1;
    }
    return rel;
}

// Create and return a Relation with odd values
Relation * oddRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    uint64_t value = 1;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = value;
        value += 2;
    }
    return rel;
}

// Create and return a Relation with even values
Relation * evenRelation(uint64_t size){
    Relation * rel = new Relation;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    uint64_t value = 0;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = value;
        value += 2;
    }
    return rel;
}


void deleteRelation(Relation * rel){
    delete[] rel->rowid;
    delete[] rel->value;
    delete rel;
}

void printRelation(Relation * rel){
    for(uint64_t i=0; i<rel->size; i++){
        std::cout << rel->rowid[i] << ". "
             << rel->value[i] << std::endl;
    }
    std::cout << std::endl;
}
