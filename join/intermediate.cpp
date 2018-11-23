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
Column * construct(Intermediate IR,
                   uint64_t relation,
                   uint64_t relColumn,
                   uint64_t * queryRelations){
    Result * res = IR.results;

    // Find the index of given relation in the intermediate results
    uint64_t intermediateIndex;
    for(uint64_t i=0; i<IR.relCount; i++){
        if(IR.relations[i] == relation){
            intermediateIndex = i;
            break;
        }
    }

    Column * constructed = newColumn(res->totalEntries);

    for(uint64_t i=0; i<res->totalEntries; i++){
        constructed->rowid[i] = i;

        uint64_t relIndex = queryRelations[IR.relations[intermediateIndex]];
        uint64_t relRowID = (getEntry(res,i,IR.relCount))[intermediateIndex];
        constructed->value[i] = r[relIndex].data[relColumn][relRowID];
    }

    return constructed;
}

// Similar to 'construct' function.
// This one is designed for self join executions. It uses the 'SelfJoinColumn'
// data structure. This way we can reduce the memory required for a self join
// by 25%, since we don't need to store rowIDs twice in a self join.
SelfJoinColumn * selfJoinConstruct(Intermediate IR,
                                   uint64_t relation,
                                   uint64_t relColumnA,
                                   uint64_t relColumnB,
                                   uint64_t * queryRelations){
    Result * res = IR.results;

    // Find the index of given relation in the intermediate results
    uint64_t intermediateIndex;
    for(uint64_t i=0; i<IR.relCount; i++){
        if(IR.relations[i] == relation){
            intermediateIndex = i;
            break;
        }
    }

    // Create new SelfJoinColumn
    SelfJoinColumn * constructed = new SelfJoinColumn();
    constructed->rowid = new uint64_t[res->totalEntries];
    constructed->valueA = new uint64_t[res->totalEntries];
    constructed->valueB = new uint64_t[res->totalEntries];
    constructed->size = res->totalEntries;

    for(uint64_t i=0; i<res->totalEntries; i++){
        constructed->rowid[i] = i;

        uint64_t relIndex = queryRelations[IR.relations[intermediateIndex]];
        uint64_t relRowID = (getEntry(res,i,IR.relCount))[intermediateIndex];

        constructed->valueA[i] = r[relIndex].data[relColumnA][relRowID];
        constructed->valueB[i] = r[relIndex].data[relColumnB][relRowID];
    }

    return constructed;
}

void deleteIntermediate(Intermediate * im){
    deleteResult(im->results);
    delete[] im->relations;
}

void deleteSJC(SelfJoinColumn * sjc){
    delete[] sjc->rowid;
    delete[] sjc->valueA;
    delete[] sjc->valueB;
    delete sjc;
}
