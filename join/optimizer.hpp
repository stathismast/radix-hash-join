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
    // After reordering the predicates keep the position of the last ordered
    // predicate
    uint64_t lastPredicate;
    std::string predicateStr;
    // A set of relations that would represent all our relations in our current
    // intermediate result
    std::vector<uint64_t> irSet;

    Stats ** myStats;

    uint64_t reorderFilters();
    uint64_t reorderFilters(uint64_t rel);
    uint64_t reorderJoins(uint64_t relA, uint64_t relB);

public:
    // constructor for the single relation nodes
    JoinTree (uint64_t rel, QueryInfo * queryInfo);
    // constructor for joins
    JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo);
    void updateGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k);

    // The set of updateIr functions update the stats for a given relation, and
    // then if that relation was in the intermediate they update the stats for
    // the rest of the relations in the intermediate since the intermediate acts
    // as a whole relation
    void updateLessFilterStatsIR(uint64_t rel, uint64_t col, uint64_t k);
    void updateGreaterFilterStatsIR(uint64_t rel, uint64_t col, uint64_t k);

    void updateStats(uint64_t rel, uint64_t col, Stats newStats);
    void updateLessFilterStats(uint64_t rel, uint64_t col, uint64_t k);
    void updateJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);

    Stats evalLessFilterStats(uint64_t rel, uint64_t col, uint64_t k);
    Stats evalGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k);
    Stats evalJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB);

    std::string getPredicateStr();
    double getCost();
    Predicate * getPredicates();
    uint64_t getPredicatesCount();
    void printStats();

    virtual ~JoinTree ();

    // deprecated and removed from the final version
    double evalCost(uint64_t rel);
    void updateJoinTree(double eval);
};

void swapPredicates(Predicate * A, Predicate * B);

bool isInVector(std::vector<uint64_t> v, uint64_t rel);
std::string vectorToString(std::vector<uint64_t> v);
std::vector<uint64_t> makeSet(std::vector<uint64_t> cur, uint64_t rel);

bool isConnected(std::vector<uint64_t> v, uint64_t rel, QueryInfo* queryInfo);

std::vector<std::vector<uint64_t>> makeRelationsSet(QueryInfo * queryInfo);
void joinEnumeration(QueryInfo * queryInfo);
