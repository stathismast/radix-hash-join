#include <cstring>
#include <cstdlib>
#include "predicates.hpp"
#include <iostream>


Filter::Filter (char* lv, char op, int rv) {
    this->op = op;
    this->value = (uint64_t) rv;
    char* dotPos = strstr(lv, ".");
    column = atoi(dotPos + 1);
    *dotPos = '\0';
    relation = atoi(lv);
}

void Filter::execute() { }

Join::Join (int relationA, int columnA, int relationB, int columnB) {
    this->relationA = (uint64_t) relationA;
    this->columnA = (uint64_t) columnA;
    this->relationB = (uint64_t) relationB;
    this->columnB = (uint64_t) columnB;
}

void Join::execute() { }

Join::~Join () { }


Filter::~Filter() { }


SelfJoin::SelfJoin (int relation, int columnA, int columnB) {
    this->relation = (uint64_t) relation;
    this->columnA = (uint64_t) columnA;
    this->columnB = (uint64_t) columnB;
}

void SelfJoin::execute() { }

SelfJoin::~SelfJoin () { }
