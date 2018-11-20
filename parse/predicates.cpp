#include <cstring>
#include <cstdlib>
#include "predicates.hpp"
#include <iostream>


Filter::Filter (char* lv, char op, int rv) {
    this->op = op;
    this->value = (uint64_t) rv;
    char * relationStr = NULL, * columnStr = NULL;
    splitAt(lv, ".", &relationStr, &columnStr);
    relation = atoi(relationStr);
    column = atoi(columnStr);
}

void Filter::execute() { }

void Filter::printInfo() {
    std::cout << "Filter ";
    std::cout << relation << "." << column;
    std::cout << " " << op << " " << value << '\n';
}


Join::Join (int relationA, int columnA, int relationB, int columnB) {
    this->relationA = (uint64_t) relationA;
    this->columnA = (uint64_t) columnA;
    this->relationB = (uint64_t) relationB;
    this->columnB = (uint64_t) columnB;
}

void Join::execute() { }

void Join::printInfo() {
    std::cout << "Join: ";
    std::cout << relationA << "." << columnA;
    std::cout << " = " << relationB << "." << columnB << '\n';
}

Join::~Join () { }


Filter::~Filter() { }


SelfJoin::SelfJoin (int relation, int columnA, int columnB) {
    this->relation = (uint64_t) relation;
    this->columnA = (uint64_t) columnA;
    this->columnB = (uint64_t) columnB;
}

void SelfJoin::execute() { }

void SelfJoin::printInfo() {
    std::cout << "SelfJoin ";
    std::cout << relation << "." << columnA;
    std::cout << " = " << relation << "." << columnB << '\n';
}

SelfJoin::~SelfJoin () { }

void splitAt(char * toSplit, char const * delim, char ** left, char ** right) {
    char* splitPos = strstr(toSplit, delim);
    *right = splitPos + 1;
    *splitPos = '\0';
    *left = toSplit;
}
