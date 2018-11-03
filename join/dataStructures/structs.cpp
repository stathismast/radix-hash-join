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

relation * segFaultTupleA(){

    tuple * array = new tuple[20];

    for(uint32_t i = 0; i<20; i++){
        array[i].rowid = i+1;
    }

    array[0].value = 6;
    array[1].value = 6;
    array[2].value = 0;
    array[3].value = 3;
    array[4].value = 9;
    array[5].value = 8;
    array[6].value = 8;
    array[7].value = 6;
    array[8].value = 0;
    array[9].value = 7;
    array[10].value = 1;
    array[11].value = 6;
    array[12].value = 4;
    array[13].value = 7;
    array[14].value = 0;
    array[15].value = 8;
    array[16].value = 6;
    array[17].value = 0;
    array[18].value = 5;
    array[19].value = 9;
    
    relation * rel = new relation;
    rel->column = array;
    rel->size = 20;
    return rel;
}

relation * segFaultTupleB(){
    tuple * array = new tuple[20];

    for(uint32_t i = 0; i<20; i++){
        array[i].rowid = i+1;
    }

    array[0].value = 1;
    array[1].value = 7;
    array[2].value = 5;
    array[3].value = 5;
    array[4].value = 1;
    array[5].value = 5;
    array[6].value = 5;
    array[7].value = 9;
    array[8].value = 3;
    array[9].value = 7;
    array[10].value = 5;
    array[11].value = 9;
    array[12].value = 3;
    array[13].value = 5;
    array[14].value = 4;
    array[15].value = 4;
    array[16].value = 3;
    array[17].value = 2;
    array[18].value = 3;
    array[19].value = 5;
    
    relation * rel = new relation;
    rel->column = array;
    rel->size = 20;
    return rel;
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
