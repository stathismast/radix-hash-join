#include "predicates.hpp"
#include "../singleJoin/join.hpp"

extern Relation * r;
extern uint64_t relationsSize;

// extern Intermediate IR;

// extern uint64_t * queryRelations;

bool compare(uint64_t x, uint64_t y, char op) {
    if (op == '<')
        return x < y;
    else if (op == '>')
        return x > y;
    else
        return x == y;

}

void execute(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    if (predicate->predicateType == FILTER) {
        executeFilter(predicate, queryRelations, IR);
    } else if (predicate->predicateType == JOIN) {
        executeJoin(predicate, queryRelations, IR);
    } else {
        executeSelfjoin(predicate, queryRelations, IR);
    }
}

// This function assumes that there is only one filter and it is applied at
// the start of the query execution only
void executeFilter(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    Relation rel = r[queryRelations[predicate->relationA]];
    Result * res = newResult();
    int column = predicate->columnA;
    uint64_t value = predicate->value;
    char op = predicate->op;
    // make the list
    for (uint64_t i = 0; i < rel.rows; i++) {
        if (compare(rel.data[column][i], value, op) ) {
            insertSingleResult(res, i);
        }
    }

    // Load results into Intermediate Results
    IR->results = res;
    IR->relCount = 1;

    uint64_t * temp = new uint64_t[1];
    temp[0] = predicate->relationA;
    IR->relations = temp;
}

void executeJoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    uint64_t relA = predicate->relationA;
    uint64_t colA = predicate->columnA;
    uint64_t relB = predicate->relationB;
    uint64_t colB = predicate->columnB;

    uint64_t relNotInIR;

    if(isInIntermediate(IR, relA) && isInIntermediate(IR, relB)){
        // std::cout << "Executing self join:" << relA << "." << colA << " = "
        //                                     << relB << "." << colB << '\n';

        Result * res = newResult();
        Column * constructedA;
        Column * constructedB;

        // Construct both columns from the intermediate results
        constructedA = construct(IR, relA, colA, queryRelations);
        constructedB = construct(IR, relB, colB, queryRelations);

        // Basically a self join, since both relations are in the IR
        for(uint64_t i = 0; i < constructedA->size; i++){
            if(constructedA->value[i] == constructedB->value[i]){
                insertSingleResult(res, i);
            }
        }

        deleteColumn(constructedA);
        deleteColumn(constructedB);

        // std::cout << "Result of join (both relations in IR) is:" << std::endl;
        // printSingleResult(res);

        // Update intermediate results
        selfJoinUpdateIR(res, IR);
        deleteResult(res);
        return;
    }

    // std::cout << "Executing join:" << relA << "." << colA << " = "
    //                                << relB << "." << colB << '\n';

    // // If one of the two relations is not in the intermediate results
    Column * fromIntermediate;
    Column * fromMappedData;

    if(isInIntermediate(IR, relA) && !isInIntermediate(IR, relB)) {
        fromIntermediate = construct(IR,relA,colA,queryRelations);
        fromMappedData = constructMappedData(relB,colB,queryRelations);
        relNotInIR = relB;
    }
    else if(!isInIntermediate(IR, relA) && isInIntermediate(IR, relB)) {
        fromMappedData = constructMappedData(relA,colA,queryRelations);
        fromIntermediate = construct(IR,relB,colB,queryRelations);
        relNotInIR = relA;
    }
    else {
        std::cout << "Error in executeJoin(). No relation is present in the "
                  << "intermediate results. This type of operation is not "
                  << "yet supported. This program will exit..." << std::endl;
        exit(0);
    }

    Result * res = join(fromIntermediate, fromMappedData);

    deleteColumn(fromIntermediate);
    delete[] fromMappedData->rowid;
    delete fromMappedData;

    // std::cout << "Join done. Results are:" << std::endl;
    // printDoubleResult(res);

    joinUpdateIR(res, relNotInIR, IR);

    deleteResult(res);
}

// In the results, the 'left' column will always be from the IR
void joinUpdateIR(Result * res, uint64_t newRel, Intermediate * IR){
    uint64_t * resEntry;
    uint64_t * IREntry;
    uint64_t * newIREntry = new uint64_t[IR->relCount+1];
    Result * newResults = newResult();

    // Iterate over the join results
    for(uint64_t i=0; i<res->totalEntries; i++){

        // Get next result
        resEntry = getEntry(res,i,2);

        // Get the row from 'intermediate' that we want to add to the new IR
        IREntry = getEntry(IR->results,resEntry[0],IR->relCount);

        // Create the new IR entry by using the previous IR entry and adding
        // the value from the most recent join at the end
        memcpy(newIREntry, IREntry, IR->relCount * sizeof(uint64_t));
        newIREntry[IR->relCount] = resEntry[1];

        // Add the old IR row to the the new IR
        insertResult(newResults,newIREntry,IR->relCount+1);
    }
    delete[] newIREntry;
    deleteResult(IR->results);

    // Update intermediate.relations array
    uint64_t * newRelations = new uint64_t[IR->relCount+1];
    memcpy(newRelations, IR->relations, IR->relCount * sizeof(uint64_t));
    newRelations[IR->relCount] = newRel;
    delete[] IR->relations;

    IR->relations = newRelations;

    // Update intermediate.relCount
    IR->relCount++;

    // Update intermidiate.results
    IR->results = newResults;
}

void executeSelfjoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    Result * res = newResult();
    uint64_t rel = predicate->relationA;
    uint64_t columnA = predicate->columnA;
    uint64_t columnB = predicate->columnB;

    // std::cout << "Executing self join:" << rel << "." << columnA << " = "
    //                                     << rel << "." << columnB << '\n';

    SelfJoinColumn * col = selfJoinConstruct(IR, rel,
                                             columnA, columnB,
                                             queryRelations);
    for(uint64_t i = 0; i < col->size; i++){
        if(col->valueA[i] == col->valueB[i]){
            // std::cout << "A:" << col->valueA[i]
            //           << " B:" << col->valueB[i] << '\n';
            insertSingleResult(res, i);
        }
    }
    deleteSJC(col);

    // std::cout << "Result of self join is:" << std::endl;
    // printSingleResult(res);

    // Update intermediate results
    selfJoinUpdateIR(res, IR);

    deleteResult(res);
}

// Update interemediate results after a self join execution
void selfJoinUpdateIR(Result * selfJoinResults, Intermediate * IR){
    uint64_t * entry;
    uint64_t * rowID;
    Result * newResults = newResult();

    // Iterate over the selfJoin results
    for(uint64_t i=0; i<selfJoinResults->totalEntries; i++){

        // Get next result (an index of a row in IR that we want to keep)
        rowID = getEntry(selfJoinResults,i,1);

        // Get the row from 'intermediate' that we want to add to the new IR
        entry = getEntry(IR->results,*rowID,IR->relCount);

        // Add the old IR row to the the new IR
        insertResult(newResults,entry,IR->relCount);
    }

    deleteResult(IR->results);

    IR->results = newResults;

    // std::cout << "Intermediate after self join:" << '\n';
    // printResult(IR.results,IR.relCount);
}

void calculateSums(QueryInfo * qi, Intermediate *IR){
    uint64_t sum = 0;

    for(uint64_t j=0; j<qi->sumsCount; j++){
        uint64_t relation = qi->sums[j].relation;
        uint64_t relColumn = qi->sums[j].column;

        uint64_t intermediateIndex;
        for(uint64_t i=0; i<IR->relCount; i++){
            if(IR->relations[i] == relation){
                intermediateIndex = i;
                break;
            }
        }

        for(uint64_t i=0; i<IR->results->totalEntries; i++){
            uint64_t relIndex = qi->relations[IR->relations[intermediateIndex]];
            uint64_t relRowID = (getEntry(IR->results,i,IR->relCount))[intermediateIndex];
            sum += r[relIndex].data[relColumn][relRowID];
        }

        std::cout << qi->sums[j].relation << "." << qi->sums[j].column
        << " sum is: " << sum << std::endl;
        sum = 0;
    }
}

void makeFilter(QueryInfo * q, int relation, int column, char op, int rv, int index) {
    q->predicates[index].op = op;
    q->predicates[index].value = (uint64_t) rv;
    q->predicates[index].relationA= (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) column;
    q->predicates[index].predicateType = FILTER;
}

void makeJoin(QueryInfo * q, int relationA, int columnA, int relationB, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relationA;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].relationB = (uint64_t) relationB;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = JOIN;
}

void makeSelfJoin(QueryInfo * q, int relation, int columnA, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = SELFJOIN;
}

void printPredicate(Predicate * predicate) {
    if (predicate->predicateType == FILTER) {
        printFilter(predicate);
    } else if (predicate->predicateType == JOIN) {
        printJoin(predicate);
    } else {
        printSelfjoin(predicate);
    }
}


void printFilter(Predicate * predicate) {
    std::cout << "Filter ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " " << predicate->op << " " << predicate->value << '\n';
}
void printJoin(Predicate * predicate) {
    std::cout << "Join: ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " = " << predicate->relationB << "." << predicate->columnB << '\n';
}
void printSelfjoin(Predicate * predicate) {
    std::cout << "SelfJoin ";
    std::cout << predicate->relationA << "." << predicate->columnA;
    std::cout << " = " << predicate->relationA << "." << predicate->columnB << '\n';
}

void deleteQueryInfo(QueryInfo * queryInfo) {
    delete[] queryInfo->relations;
    delete[] queryInfo->predicates;
    delete[] queryInfo->sums;
    delete queryInfo;
}
