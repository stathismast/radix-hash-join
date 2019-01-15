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

unsigned long long currentTime() {
    struct timeval te;
    gettimeofday(&te, 0);
    return te.tv_sec*1000000 + te.tv_usec;
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
    TIMEVAR startTime = currentTime();

    Relation rel = r[queryRelations[predicate->relationA]];
    Result * res = newResult();
    int column = predicate->columnA;
    uint64_t value = predicate->value;
    char op = predicate->op;

    // make the list
    if(op == '<'){
        for(uint64_t i=0; i<rel.rows; i++)
            if(rel.data[column][i] < value)
                insertSingleResult(res, i);
    }
    else if(op == '>'){
        for(uint64_t i=0; i<rel.rows; i++)
            if(rel.data[column][i] > value)
                insertSingleResult(res, i);
    }
    else{
        for(uint64_t i=0; i<rel.rows; i++)
            if(rel.data[column][i] == value)
                insertSingleResult(res, i);
    }

    // Load results into Intermediate Results
    IR->results[predicate->relationA] = fastResultToArray(res);
    IR->length = res->totalEntries;

    deleteResult(res);

    std::cerr << "Filter: " << predicate->relationA << "." << column << " "
              << op << " " << value
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << IR->length << " entries)" << '\n';
}

void executeJoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    if(isEmpty(IR)){
        executeNoFilterJoin(predicate, queryRelations, IR);
        return;
    }

    uint64_t relA = predicate->relationA;
    uint64_t colA = predicate->columnA;
    uint64_t relB = predicate->relationB;
    uint64_t colB = predicate->columnB;

    uint64_t relNotInIR;

    if(isInIntermediate(IR, relA) && isInIntermediate(IR, relB)){
        TIMEVAR startTime = currentTime();

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

        // Update intermediate results
        selfJoinUpdateIR(res, IR);

        std::cerr << "Secondary Self Join: " << relA << "." << colA << " = "
                  << relB << "." << colB
        << " (" << ((double)(currentTime() - startTime))/1000000
        << " seconds, " << IR->length << " entries)" << '\n';

        deleteResult(res);

        return;
    }

    TIMEVAR startTime = currentTime();

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
        std::cerr << "Error in executeJoin(). No relation is present in the "
                  << "intermediate results. This type of operation is not "
                  << "yet supported. This program will exit..." << std::endl;
        exit(0);
    }


    std::cerr << "Constructs: "
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << fromIntermediate->size
    << "/" << fromMappedData->size << " entries)" << '\n';

    startTime = currentTime();

    Result ** res = join(fromIntermediate, fromMappedData);

    std::cerr << "Join: " << relA << "." << colA << " = "
                         << relB << "." << colB
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << IR->length << " entries)" << '\n';

    deleteColumn(fromIntermediate);
    delete[] fromMappedData->rowid;
    delete fromMappedData;

    // std::cerr << "Join done. Results are:" << std::endl;
    // printDoubleResult(res);


    startTime = currentTime();
    joinUpdateIR(res, relNotInIR, IR);

    std::cerr << "Total Update IR: "
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds)" << '\n';

    deleteResult(res[0]);
    deleteResult(res[1]);
    delete[] res;
}

// In the results, the 'left' column will always be from the IR
void joinUpdateIR(Result ** res, uint64_t newRel, Intermediate * IR){
    uint64_t newLength = res[0]->totalEntries;

    TIMEVAR startTime = currentTime();
    // Convert the results of the most recent join into an array
    uint64_t * fromIntermediate = fastResultToArray(res[0]);
    std::cerr << "Conversion from IR to array"
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << newLength << " entries)" << '\n';

    startTime = currentTime();
    uint64_t * fromMappedData = fastResultToArray(res[1]);
    std::cerr << "Conversion from mapped data to array"
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << newLength << " entries)" << '\n';

    // Run through the exising results in the IR and
    // update them based on the latest join results

    startTime = currentTime();
    for(uint64_t i=0; i<4; i++){
        if(IR->results[i] == NULL) continue;
        uint64_t * temp = IR->results[i];
        IR->results[i] = new uint64_t[newLength];
        for(uint64_t j=0; j<newLength; j++){
            IR->results[i][j] = temp[fromIntermediate[j]];
        }
        delete[] temp;
    }
    delete[] fromIntermediate;

    std::cerr << "IR replacement: "
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << newLength << " entries)" << '\n';

    IR->results[newRel] = fromMappedData;
    IR->length = newLength;
}

void executeNoFilterJoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    uint64_t relA = predicate->relationA;
    uint64_t colA = predicate->columnA;
    uint64_t relB = predicate->relationB;
    uint64_t colB = predicate->columnB;

    TIMEVAR startTime = currentTime();

    // // If one of the two relations is not in the intermediate results
    Column * constructedA;
    Column * constructedB;

    constructedA = constructMappedData(relA,colA,queryRelations);
    constructedB = constructMappedData(relB,colB,queryRelations);


    std::cerr << "No Filter Constructs: "
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << constructedA->size
    << "/" << constructedA->size << " entries)" << '\n';

    startTime = currentTime();

    Result ** res = join(constructedA, constructedB);

    std::cerr << "No Filter Join: " << relA << "." << colA << " = "
                         << relB << "." << colB
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << res[0]->totalEntries << " entries)" << '\n';

    delete[] constructedA->rowid;
    delete constructedA;
    delete[] constructedB->rowid;
    delete constructedB;

    // std::cerr << "Join done. Results are:" << std::endl;
    // printDoubleResult(res);


    startTime = currentTime();

    // Update intermediate results
    IR->length = res[0]->totalEntries;
    IR->results[relA] = fastResultToArray(res[0]);
    IR->results[relB] = fastResultToArray(res[1]);

    std::cerr << "Total Update IR: "
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds)" << '\n';

    deleteResult(res[0]);
    deleteResult(res[1]);
    delete[] res;
}

void executeSelfjoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR) {
    if(isEmpty(IR)){
        executeNoFilterSelfjoin(predicate, queryRelations, IR);
        return;
    }

    TIMEVAR startTime = currentTime();

    Result * res = newResult();
    uint64_t rel = predicate->relationA;
    uint64_t columnA = predicate->columnA;
    uint64_t columnB = predicate->columnB;

    SelfJoinColumn * col = selfJoinConstruct(IR, rel,
                                             columnA, columnB,
                                             queryRelations);
    for(uint64_t i = 0; i < col->size; i++){
        if(col->valueA[i] == col->valueB[i]){
            // std::cerr << "A:" << col->valueA[i]
            //           << " B:" << col->valueB[i] << '\n';
            insertSingleResult(res, i);
        }
    }
    deleteSJC(col);

    // std::cerr << "Result of self join is:" << std::endl;
    // printSingleResult(res);

    // Update intermediate results
    selfJoinUpdateIR(res, IR);

    std::cerr << "Self Join: " << rel << "." << columnA << " = "
                               << rel << "." << columnB
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << IR->length << " entries)" << '\n';

    deleteResult(res);
}

// Update interemediate results after a self join execution
void selfJoinUpdateIR(Result * selfJoinResults, Intermediate * IR){
    uint64_t newLength = selfJoinResults->totalEntries;

    // Convert the results of the most recent self join into an array
    uint64_t * rowIDs = fastResultToArray(selfJoinResults);

    // Run through the exising results in the IR and
    // update them based on the latest self join results
    for(uint64_t i=0; i<4; i++){
        if(IR->results[i] == NULL) continue;
        uint64_t * temp = IR->results[i];
        IR->results[i] = new uint64_t[newLength];
        for(uint64_t j=0; j<newLength; j++){
            IR->results[i][j] = temp[rowIDs[j]];
        }
        delete[] temp;
    }
    delete[] rowIDs;

    IR->length = newLength;
}

void executeNoFilterSelfjoin(Predicate * predicate, uint64_t * queryRelations, Intermediate * IR){
    TIMEVAR startTime = currentTime();

    Result * res = newResult();
    uint64_t rel = predicate->relationA;
    uint64_t columnA = predicate->columnA;
    uint64_t columnB = predicate->columnB;

    SelfJoinColumn * col = selfJoinConstructMappedData(IR, rel,
                                                       columnA, columnB,
                                                       queryRelations);
    for(uint64_t i = 0; i < col->size; i++){
        if(col->valueA[i] == col->valueB[i]){
            // std::cerr << "A:" << col->valueA[i]
            //           << " B:" << col->valueB[i] << '\n';
            insertSingleResult(res, i);
        }
    }
    delete[] col->rowid;
    delete col;

    // std::cerr << "Result of self join is:" << std::endl;
    // printSingleResult(res);

    // Update intermediate results
    uint64_t * resultsArray = fastResultToArray(res);
    IR->length = res->totalEntries;
    IR->results[rel] = resultsArray;

    std::cerr << "No Filter Self Join: " << rel << "." << columnA << " = "
                               << rel << "." << columnB
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << IR->length << " entries)" << '\n';

    deleteResult(res);
}

void calculateSums(QueryInfo * qi, Intermediate *IR){
    uint64_t sum = 0;

    for(uint64_t j=0; j<qi->sumsCount; j++){
        TIMEVAR startTime = currentTime();

        uint64_t relation = qi->sums[j].relation;
        uint64_t relColumn = qi->sums[j].column;

        for(uint64_t i=0; i<IR->length; i++){
            uint64_t relIndex = qi->relations[relation];
            uint64_t relRowID = IR->results[relation][i];
            sum += r[relIndex].data[relColumn][relRowID];
        }

        std::cerr << "Sum " << qi->sums[j].relation << "."
        << qi->sums[j].column << ": " << sum
    << " (" << ((double)(currentTime() - startTime))/1000000
    << " seconds, " << IR->length << " entries)" << '\n';

    if(j == 0){
        writeSum(sum);
    }
    else{
        writeWhitespace();
        writeSum(sum);
    }

        sum = 0;
    }

    writeNewLine();
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
    std::cerr << "Filter ";
    std::cerr << predicate->relationA << "." << predicate->columnA;
    std::cerr << " " << predicate->op << " " << predicate->value << '\n';
}
void printJoin(Predicate * predicate) {
    std::cerr << "Join: ";
    std::cerr << predicate->relationA << "." << predicate->columnA;
    std::cerr << " = " << predicate->relationB << "." << predicate->columnB << '\n';
}
void printSelfjoin(Predicate * predicate) {
    std::cerr << "SelfJoin ";
    std::cerr << predicate->relationA << "." << predicate->columnA;
    std::cerr << " = " << predicate->relationA << "." << predicate->columnB << '\n';
}

void deleteQueryInfo(QueryInfo * queryInfo) {
    delete[] queryInfo->relations;
    delete[] queryInfo->predicates;
    delete[] queryInfo->sums;
    delete queryInfo;
}

void copyPredicates(Predicate ** target, Predicate * source, uint64_t count) {
    // uint64_t count = queryInfo->predicatesCount;
    *target = new Predicate[count];
    for (size_t i = 0; i < count; i++) {
        (*target)[i] = source[i];
    }
}

void writeSum(uint64_t sum){
    char buffer[64] = {0};
    sprintf(buffer, "%lu", sum);

    if(sum == 0)
        sprintf(buffer, "NULL");

    ssize_t retval = write(1,buffer,strlen(buffer));
    std::cerr << "Wrote " << retval << " bytes to stdout" << std::endl;
    // fprintf(stderr, "Write: -%s-\n", buffer);
}

void writeWhitespace(){
    char c = ' ';
    ssize_t retval = write(1, &c, 1);
    std::cerr << "Wrote " << retval << " bytes to stdout" << std::endl;
}

void writeNewLine(){
    char c = '\n';
    ssize_t retval = write(1, &c, 1);
    std::cerr << "Wrote " << retval << " bytes to stdout" << std::endl;
}

char * readLine(){
    char * line = new char[1024];
    for(int i=0; i<1024; i++)
        line[i] = 0;

    char c;
    int pos = 0;
    while(read(0,&c,1) > 0){
        if(c == '\n') break;
        line[pos] = c;
        pos++;
    }

    return line;
}
