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
// colIR: the column in the intermediate results that we want to use
// colRel: the colmun of the relation that we want to use in the join
// queryRelations: an array to convert relative indexes to absolute indexes
// Example:
// Let's asssume that the intermediate results contain relations 4 and 3.
// Then let's assume that we want to execute the following join: 3.5 = 7.1
// 3.5 should be constructed from the intermediate results since 3 is included
// in the second position in the intermediate results.
// In that case, colIR will be 1, since relation 3 is at position 1 in the intermediate results
// intermediate results and colRel will be 2 since we want the 
Column * construct(Intermediate IR,
                        uint64_t colIR,
                        uint64_t colRel,
                        uint64_t * queryRelations){
    Result * res = IR.results;
    
    // Column should be in the range [0, entryCount-1]
    if(colIR >= IR.relCount) return NULL;

    Column * constructed = newColumn(res->totalEntries);

    for(uint64_t i=0; i<res->totalEntries; i++){
        constructed->rowid[i] = i;

        uint64_t relIndex = queryRelations[IR.relations[colIR]];
        uint64_t relColumn = colRel;
        uint64_t relRowID = (getEntry(res,i,IR.relCount))[colIR];

        constructed->value[i] = r[relIndex].data[relColumn][relRowID];
    }

    return constructed;
}

void deleteIntermediate(Intermediate * im){
    deleteResult(im->results);
    delete[] im->relations;
}
