#include <string>
#include <vector>
#include "predicates.hpp"
#include "stats.hpp"

class JoinTree {
private:
    double cost;
    uint64_t * relations;
    uint64_t relationsCount;
    Predicate * predicates;
    uint64_t predicatesCount;
    std::string predicateStr;
    std::vector<uint64_t> set;
    uint64_t lastPredicate;
    Stats ** myStats;

    uint64_t reorderFilters(uint64_t rel);
    uint64_t reorderJoins(uint64_t relA, uint64_t relB);

public:
    // constructor for the single relation nodes
    JoinTree (uint64_t rel, QueryInfo * queryInfo);
    // constructor for joins when there is no tree for the v set
    JoinTree (std::vector<uint64_t> v, uint64_t rel, QueryInfo * queryInfo);
    // constructor for joins
    JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo);
    void updateJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);
    void updateJoinTree(double eval);
    std::string getPredicateStr();
    double getCost();
    Predicate * getPredicates();
    uint64_t getPredicatesCount();
    void printStats();
    virtual ~JoinTree ();
};

void swapPredicates(Predicate * A, Predicate * B);

bool isInVector(std::vector<uint64_t> v, uint64_t rel);
std::string vectorToString(std::vector<uint64_t> v);
std::vector<uint64_t> makeSet(std::vector<uint64_t> cur, uint64_t rel);

bool isConnected(std::vector<uint64_t> v, uint64_t rel, QueryInfo* queryInfo);

std::vector<std::vector<uint64_t>> makeRelationsSet(QueryInfo * queryInfo);
void joinEnumeration(QueryInfo * queryInfo);
