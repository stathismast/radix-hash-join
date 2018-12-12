#include "intermediate.hpp"

extern Relation * r;

// Construct a Column struct from a given column in a Result data structure
// VERY IMPORTANT: RowIDs start from 1.
// VERY IMPORTANT: Argument 'col' starts from 0.
Column * resultToColumn(Result * res, uint64_t col, uint64_t entryCount){
    // Column should be in the range [0, entryCount-1]
    if(col >= entryCount) return NULL;

    Column * column = newColumn(res->totalEntries);

    for(uint64_t i=0; i<res->totalEntries; i++){
        column->rowid[i] = i+1;
        column->value[i] = (getEntry(res,i,entryCount))[col];
    }

    return column;
}

// IR: the intermediate results that we want to use to construct a column
// relation: the relative relation index
// relColumn: the colmun of the relation that we want to use in the join
// queryRelations: an array to convert relative indexes to absolute indexes
// Example:
// If we want to construct the relative 3.5 column then 'relation' will be 3
// and 'relColumn' will be 5
Column * construct(Intermediate * IR,
                   uint64_t relation,
                   uint64_t relColumn,
                   uint64_t * queryRelations){

    Column * constructed = newColumn(IR->length);

    for(uint64_t i=0; i<IR->length; i++){
        constructed->rowid[i] = i;

        uint64_t relIndex = queryRelations[relation];
        uint64_t relRowID = IR->results[relation][i];
        constructed->value[i] = r[relIndex].data[relColumn][relRowID];
    }

    return constructed;
}

// Similar to 'construct' function.
// This one is designed for self join executions. It uses the 'SelfJoinColumn'
// data structure. This way we can reduce the memory required for a self join
// by 25%, since we don't need to store rowIDs twice in a self join.
SelfJoinColumn * selfJoinConstruct(Intermediate * IR,
                                   uint64_t relation,
                                   uint64_t relColumnA,
                                   uint64_t relColumnB,
                                   uint64_t * queryRelations){

    // Create new SelfJoinColumn
    SelfJoinColumn * constructed = new SelfJoinColumn();
    constructed->rowid = new uint64_t[IR->length];
    constructed->valueA = new uint64_t[IR->length];
    constructed->valueB = new uint64_t[IR->length];
    constructed->size = IR->length;

    for(uint64_t i=0; i<IR->length; i++){
        constructed->rowid[i] = i;

        uint64_t relIndex = queryRelations[relation];
        uint64_t relRowID = IR->results[relation][i];

        constructed->valueA[i] = r[relIndex].data[relColumnA][relRowID];
        constructed->valueB[i] = r[relIndex].data[relColumnB][relRowID];
    }

    return constructed;
}

// This function is used when a relation is not in the intermediate results
// but we need to use it for a join
Column * constructMappedData(uint64_t relIndex,
                             uint64_t column,
                             uint64_t * queryRelations){

    // Convert relative index to actual index in mapped data
    relIndex = queryRelations[relIndex];

    // Create new column and give it a pointer to the appropriate mapped data
    Column * constructed = new Column;
    constructed->value = r[relIndex].data[column];
    constructed->size = r[relIndex].rows;

    constructed->rowid = new uint64_t[r[relIndex].rows];
    for(uint64_t i=0; i<r[relIndex].rows; i++){
        constructed->rowid[i] = i;
    }

    return constructed;
}

void deleteIntermediate(Intermediate * im){
    for(uint64_t i=0; i<4; i++){
        if(im->results[i] != NULL)
            delete[] im->results[i];
    }

    delete im;
}

void deleteSJC(SelfJoinColumn * sjc){
    delete[] sjc->rowid;
    delete[] sjc->valueA;
    delete[] sjc->valueB;
    delete sjc;
}

bool isInIntermediate(Intermediate * intermediate, uint64_t relation){
    return (intermediate->results[relation] != NULL);
}

// Convert a Result with a single entry into an array
uint64_t * singleResultToArray(Result * res){
    uint64_t * array = new uint64_t[res->totalEntries];
    for(uint64_t i=0; i<res->totalEntries; i++){
        array[i] = getSingleEntry(res,i);
    }
    return array;
}

// Convert the second entry of a Result with two entries into an array
uint64_t * resultToArray(Result * res, uint64_t count, uint64_t index){
    uint64_t * array = new uint64_t[res->totalEntries];
    for(uint64_t i=0; i<res->totalEntries; i++){
        array[i] = getEntry(res,i,count)[index];
    }
    return array;
}
