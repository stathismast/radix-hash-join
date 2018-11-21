#include "intermediate.hpp"

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

void deleteIntermediate(Intermediate * im){
    deleteResult(im->results);
    delete[] im->relations;
}
