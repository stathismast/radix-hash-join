#include <string>
#include "predicates.hpp"

class PredicateNode {
private:
    double cost;
    Predicate * predicates;
    uint64_t predicatesCount;
    std::string predicateStr;
    uint64_t lastPredicate;
    uint64_t reorderFilters(uint64_t rel);
    uint64_t reorderJoins();

public:
    PredicateNode (std::string a, uint64_t b);
    // constructor for the single relation nodes
    PredicateNode (uint64_t rel, QueryInfo * queryInfo);
    // constructor for joins
    PredicateNode (PredicateNode * n, uint64_t rel, QueryInfo * queryInfo);
    std::string getPredicateStr();
    double getCost();
    virtual ~PredicateNode ();
};


void createRelationsSet(QueryInfo* queryInfo);
void joinEnumeration(QueryInfo * queryInfo);
