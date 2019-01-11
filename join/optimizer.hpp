#include <string>
#include <vector>
#include "predicates.hpp"
#include "stats.hpp"

class JoinTree {
private:
    double cost;
    Predicate * predicates;
    uint64_t predicatesCount;
    std::string predicateStr;
    uint64_t lastPredicate;
    std::vector<uint64_t> set;

    uint64_t reorderFilters(uint64_t rel);
    uint64_t reorderJoins(uint64_t relA, uint64_t relB);

public:
    JoinTree (std::string a, uint64_t b);
    JoinTree (std::vector<uint64_t> v, uint64_t rel, QueryInfo * queryInfo);
    // constructor for the single relation nodes
    JoinTree (uint64_t rel, QueryInfo * queryInfo);
    // constructor for joins
    JoinTree (JoinTree * n, uint64_t rel, QueryInfo * queryInfo);
    // JoinTree (JoinTree * n, QueryInfo * queryInfo);
    void updateJoinTree(double eval);
    std::string getPredicateStr();
    double getCost();
    virtual ~JoinTree ();
};

std::vector<std::vector<uint64_t>> makeRelationsSet(QueryInfo * queryInfo);
std::vector<uint64_t> makeSet(std::vector<uint64_t> cur, uint64_t rel);
std::string vectorToString(std::vector<uint64_t> v);
void swapPredicates(Predicate * A, Predicate * B);
void createRelationsSet(QueryInfo* queryInfo);
void joinEnumeration(QueryInfo * queryInfo);
