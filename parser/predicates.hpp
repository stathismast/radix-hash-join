#include <stdint.h>

#ifndef PREDICATES_HPP
#define PREDICATES_HPP

class Predicate {
    public:
        Predicate () {};
        virtual void execute() = 0;
        virtual void printInfo() = 0;
        virtual ~Predicate () {};
};

class Filter: public Predicate {
    private:
        uint64_t relation;
        uint64_t column;
        char op;
        uint64_t value;

    public:
        Filter (char* lv, char op, int rv);
        void execute();
        void printInfo();
        virtual ~Filter ();
};

class Join: public Predicate {
    private:
        uint64_t relationA;
        uint64_t columnA;
        uint64_t relationB;
        uint64_t columnB;

    public:
        Join (int relationA, int columnA, int relationB, int columnB);
        void execute();
        void printInfo();
        virtual ~Join ();
};

class SelfJoin: public Predicate {
    private:
        uint64_t relation;
        uint64_t columnA;
        uint64_t columnB;

    public:
        SelfJoin (int relation, int columnA, int columnB);
        void execute();
        void printInfo();
        virtual ~SelfJoin ();
};

void splitAt(char * toSplit, char const * delim, char ** left, char ** right);
int countArgs(char * in, char const * delim);

#endif
