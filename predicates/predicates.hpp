#include <stdint.h>

#ifndef PREDICATES_HPP
#define PREDICATES_HPP

class Predicate {
    public:
        Predicate ();
        virtual void execute() = 0;
        virtual ~Predicate ();
};

class Filter: public Predicate {
    private:
        uint64_t relation;
        uint64_t column;
        char op;

    public:
        Filter ();
        void execute();
        virtual ~Filter ();
};

class Join: public Predicate {
    private:
        uint64_t relationA;
        uint64_t columnA;
        uint64_t relationB;
        uint64_t columnB;

    public:
        Join ();
        void execute();
        virtual ~Join ();
};

class SelfJoin: public Predicate {
    private:
        uint64_t relation;
        uint64_t columnA;
        uint64_t columnB;

    public:
        SelfJoin ();
        void execute();
        virtual ~SelfJoin ();
};

#endif
