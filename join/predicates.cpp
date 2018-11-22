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

    printSingleResult(res);

    //keep necessary rowids rows from old Intermediate
    //TO DO Need isRelationIn function because we need intermediate.relCount
    //+1 if !isRelationIn. Not maybe at this type of predicate but in general
    uint64_t * tempRow;
    uint64_t currentRowId;
    Result * newRes = newResult();
    //save old intermediate info
    uint64_t oldRelCount = intermediate.relCount;
    //iterate over new RowIds
    for( uint64_t i = 0; i < res->totalEntries; i ++ ){
        //get i_th new Rowid
        currentRowId = getEntry(res,i,1)[0];
        //get corresponding currentRowId row from old intermediate
        tempRow = getEntry(intermediate.results,currentRowId,oldRelCount);

        //add new row in new intermediate
        //TO DO In other predicates, if !isRelationIn we need to append the new
        //real RowId at the end of the row before the insertion
        //so it will be oldRelCount + 1
        insertResult(newRes,tempRow,oldRelCount);
    }


    deleteColumn(colA);
    deleteColumn(colB);
    deleteIntermediate(&intermediate);
    deleteResult(res);
    
    // Load results into Intermediate Results
    intermediate.results = newRes;
    intermediate.relCount = oldRelCount; //or oldRelCount + 1 if !isRelationIn

    //TO DO we need to save old relations before deleteIntermediate
    uint64_t * temp = new uint64_t[1];
    temp[0] = predicate->relationA;
    // temp[0] = queryRelations[predicate->relationA];
    intermediate.relations = temp;

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
