#include "predicates.hpp"

extern Relation * r;
extern uint64_t relationsSize;

extern Intermediate intermediate;

// extern uint64_t * queryRelations;

bool compare(uint64_t x, uint64_t y, char op) {
    if (op == '<')
        return x < y;
    else if (op == '>')
        return x > y;
    else
        return x == y;

}

void execute(Predicate * predicate, uint64_t * queryRelations) {
    if (predicate->predicateType == 1) {
        executeFilter(predicate, queryRelations);
    } else if (predicate->predicateType == 2) {
        executeJoin(predicate, queryRelations);
    } else {
        executeSelfjoin(predicate, queryRelations);
    }
}

// This function assumes that there is only one filter and it is applied at
// the start of the query execution only
void executeFilter(Predicate * predicate, uint64_t * queryRelations) {
    // if(relationsSize) printData(r[relation]);
    // Relation rel = r[queryRelations[predicate->relationA]];
    Relation rel = r[queryRelations[predicate->relationA]];
    Result * res = newResult();
    int column = predicate->columnA;
    uint64_t value = predicate->value;
    char op = predicate->op;
    // make the list
    for (uint64_t i = 0; i < rel.rows; i++) {
        if (compare(rel.data[column][i], value, op) ) {
            // std::cout << rel.data[column][i] << '\n';
            insertSingleResult(res, i);
        }
    }

    // Load results into Intermediate Results
    intermediate.results = res;
    intermediate.relCount = 1;

    uint64_t * temp = new uint64_t[1];
    temp[0] = predicate->relationA;
    // temp[0] = queryRelations[predicate->relationA];
    intermediate.relations = temp;
}

void executeJoin(Predicate * predicate, uint64_t * queryRelations) {
    std::cout << "Join" << '\n';
}

void executeSelfjoin(Predicate * predicate, uint64_t * queryRelations) {
    // Relation rel = r[queryRelations[predicate->relationA]];
    Result * res = newResult();
    uint64_t rel = predicate->relationA;
    uint64_t columnA = predicate->columnA;
    uint64_t columnB = predicate->columnB;

    Column * colA = construct(intermediate, rel, columnA, queryRelations);
    Column * colB = construct(intermediate, rel, columnB, queryRelations);
    for (uint64_t i = 0; i < colA->size; i++) {
        // std::cout << "A:" << colA->value[i] << " B:" << colB->value[i] << '\n';
        if (compare(colA->value[i], colB->value[i], '=')) {
            std::cout << "A:" << colA->value[i] << " B:" << colB->value[i] << '\n';
            insertSingleResult(res, i);
        }
    }
    deleteColumn(colA);
    deleteColumn(colB);

    std::cout << "Result of self join is:" << std::endl;
    printSingleResult(res);


    uint64_t * entry;
    uint64_t rowID;
    Result * newResults = newResult();

    // Iterate over the selfJoin results
    for(uint64_t i=0; i<res->totalEntries; i++){
        
        // Get next result
        rowID = getEntry(res,i,1)[0];

        // Get the row from 'intermediate' that we want to add to the new IR
        entry = getEntry(intermediate.results,rowID,intermediate.relCount);

        // Add the old IR row to the the new IR
        insertResult(newResults,entry,intermediate.relCount);
    }

    deleteResult(intermediate.results);
    deleteResult(res);
    
    // Load results into Intermediate Results
    intermediate.results = newResults;

    std::cout << "Final Intermediate:" << '\n';
    printResult(intermediate.results,intermediate.relCount);
}

void makeFilter(QueryInfo * q, int relation, int column, char op, int rv, int index) {
    q->predicates[index].op = op;
    q->predicates[index].value = (uint64_t) rv;
    q->predicates[index].relationA= (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) column;
    q->predicates[index].predicateType = 1;
}

void makeJoin(QueryInfo * q, int relationA, int columnA, int relationB, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relationA;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].relationB = (uint64_t) relationB;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = 2;
}

void makeSelfJoin(QueryInfo * q, int relation, int columnA, int columnB, int index) {
    q->predicates[index].relationA = (uint64_t) relation;
    q->predicates[index].columnA = (uint64_t) columnA;
    q->predicates[index].columnB = (uint64_t) columnB;
    q->predicates[index].predicateType = 3;
}

void printPredicate(Predicate * predicate) {
    if (predicate->predicateType == 1) {
        printFilter(predicate);
    } else if (predicate->predicateType == 2) {
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
