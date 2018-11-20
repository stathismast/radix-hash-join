#include "structs.hpp"

Column * newColumn(uint64_t size){
    Column * rel = new Column;
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

// Create and return a Column with serial values
Column * randomColumn(uint64_t size){
    Column * rel = new Column;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = rand()%2000;
    }
    return rel;
}

// Create and return a Column with serial values
Column * serialColumn(uint64_t size){
    Column * rel = new Column;
    rel->rowid = new uint64_t[size];
    rel->value = new uint64_t[size];

    rel->size = size;
    for(uint64_t i = 0; i<size; i++){
        rel->rowid[i] = i+1;
        rel->value[i] = i+1;
    }
    return rel;
}

// Create and return a Column with odd values
Column * oddColumn(uint64_t size){
    Column * rel = new Column;
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

// Create and return a Column with even values
Column * evenColumn(uint64_t size){
    Column * rel = new Column;
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


void deleteColumn(Column * rel){
    delete[] rel->rowid;
    delete[] rel->value;
    delete rel;
}

void printColumn(Column * rel){
    for(uint64_t i=0; i<rel->size; i++){
        std::cout << rel->rowid[i] << ". "
             << rel->value[i] << std::endl;
    }
    std::cout << std::endl;
}
