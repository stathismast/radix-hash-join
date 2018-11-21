#include <stdint.h>

#ifndef PREDICATES_HPP
#define PREDICATES_HPP

typedef struct Predicate {
    uint64_t relationA;
    uint64_t columnA;
    uint64_t relationB;
    uint64_t columnB;
    char op;
    uint64_t value;
    char predicateType;
} Predicate;


// class Predicate {
//     public:
//         Predicate () {};
//         virtual void execute() = 0;
//         virtual void printInfo() = 0;
//         virtual ~Predicate () {};
// };
//
// class Filter: public Predicate {
//     private:
//         uint64_t relation;
//         uint64_t column;
//
//         uint64_t value;
//
//     public:
//         Filter (int relation, int column, char op, int rv);
//         void execute();
//         void printInfo();
//         virtual ~Filter ();
// };
//
// class Join: public Predicate {
//     private:
//         uint64_t relationA;
//         uint64_t columnA;
//         uint64_t relationB;
//         uint64_t columnB;
//
//     public:
//         Join (int relationA, int columnA, int relationB, int columnB);
//         void execute();
//         void printInfo();
//         virtual ~Join ();
// };
//
// class SelfJoin: public Predicate {
//     private:
//         uint64_t relation;
//         uint64_t columnA;
//         uint64_t columnB;
//
//     public:
//         SelfJoin (int relation, int columnA, int columnB);
//         void execute();
//         void printInfo();
//         virtual ~SelfJoin ();
// };

#endif
