#include <cstring>
#include <cstdlib>
#include "predicates.hpp"
#include <iostream>


Filter::Filter (int relation, int column, char op, int rv) {
    this->op = op;
    this->value = (uint64_t) rv;
    this->relation = relation;
    this->column = column;
}

void Filter::execute() { }

void Filter::printInfo() {
    std::cout << "Filter ";
    std::cout << this->relation << "." << this->column;
    std::cout << " " << this->op << " " << this->value << '\n';
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
    std::cout << this->relationA << "." << this->columnA;
    std::cout << " = " << this->relationB << "." << this->columnB << '\n';
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
    std::cout << this->relation << "." << this->columnA;
    std::cout << " = " << this->relation << "." << this->columnB << '\n';
}

SelfJoin::~SelfJoin () { }
