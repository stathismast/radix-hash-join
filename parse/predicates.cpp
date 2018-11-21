#include <cstring>
#include <cstdlib>
#include "predicates.hpp"
#include "../join/memmap.hpp"
#include "../singleJoin/result.hpp"
#include "parse.hpp"
#include <iostream>

//global
extern Relation * r;
extern uint64_t relationsSize;

bool compare(uint64_t x, uint64_t y, char op) {
    if (op == '<')
        return x < y;
    else if (op == '>')
        return x > y;
    else
        return x == y;

}

void executeFilter(Predicate * predicate) {
    // if(relationsSize) printData(r[relation]);
    Relation rel = r[predicate->relationA];
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
    printSingleResult(res);
}

void executeJoin(Predicate * predicate) {

}

void executeSelfjoin(Predicate * predicate) {

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

void execute(Predicate * predicate) {
    if (predicate->predicateType == 1) {
        executeFilter(predicate);
    } else if (predicate->predicateType == 2) {
        executeJoin(predicate);
    } else {
        executeSelfjoin(predicate);
    }
}
// //-----------------------------FILTER---------------------------------------------
// Filter::Filter (int relation, int column, char op, int rv) {
//     this->op = op;
//     this->value = (uint64_t) rv;
//     this->relation = (uint64_t) relation;
//     this->column = (uint64_t) column;
// }

// void Filter::execute() {
//     // if(relationsSize) printData(r[relation]);
//     Relation rel = r[relation];
//     Result * res = newResult();
//     // make the list
//     for (uint64_t i = 0; i < rel.rows; i++) {
//         if (compare(rel.data[column][i], value, op) ) {
//             // std::cout << rel.data[column][i] << '\n';
//             insertSingleResult(res, i);
//         }
//     }
//     printSingleResult(res);
// }

// void Filter::printInfo() {
//     std::cout << "Filter ";
//     std::cout << this->relation << "." << this->column;
//     std::cout << " " << this->op << " " << this->value << '\n';
// }
//
// Filter::~Filter() {}


//-----------------------------JOIN---------------------------------------------
// Join::Join (int relationA, int columnA, int relationB, int columnB) {
//     this->relationA = (uint64_t) relationA;
//     this->columnA = (uint64_t) columnA;
//     this->relationB = (uint64_t) relationB;
//     this->columnB = (uint64_t) columnB;
// }

// void Join::execute() { }
//
// void Join::printInfo() {
//     std::cout << "Join: ";
//     std::cout << this->relationA << "." << this->columnA;
//     std::cout << " = " << this->relationB << "." << this->columnB << '\n';
// }
//
// Join::~Join () { }


//-----------------------------SELF JOIN----------------------------------------
// SelfJoin::SelfJoin (int relation, int columnA, int columnB) {
//     this->relation = (uint64_t) relation;
//     this->columnA = (uint64_t) columnA;
//     this->columnB = (uint64_t) columnB;
// }
//
// void SelfJoin::execute() { }
//
// void SelfJoin::printInfo() {
//     std::cout << "SelfJoin ";
//     std::cout << this->relation << "." << this->columnA;
//     std::cout << " = " << this->relation << "." << this->columnB << '\n';
// }
//
// SelfJoin::~SelfJoin () { }
