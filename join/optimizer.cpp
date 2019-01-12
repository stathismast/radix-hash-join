#include "optimizer.hpp"
#include "predicates.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include "stats.hpp"
#include <unordered_map>
#include <unordered_set>

using std::string;

extern Stats ** stats;

// constructor for the single relation nodes
JoinTree::JoinTree (uint64_t rel, QueryInfo * queryInfo) {
    this->cost = stats[rel][0].d;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, queryInfo->predicates, this->predicatesCount);

    this->predicateStr = std::to_string(rel);
    this->set = {rel};

    this->lastPredicate = 0;
    this->lastPredicate = reorderFilters(rel);

    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&this->predicates[i]);
    // }
    // std::cout << '\n';
    // std::cout << "las = " << this->lastPredicate << '\n';
}


// Constructor for joins when there is no tree for the v set
JoinTree::JoinTree (std::vector<uint64_t> v, uint64_t rel, QueryInfo * queryInfo) {
    this->cost = 0;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, queryInfo->predicates, this->predicatesCount);

    this->set = makeSet(v, rel);
    this->predicateStr = vectorToString(this->set);

    this->lastPredicate = 0;

    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = 0; i < this->predicatesCount; i++) {
        // Find which relatios are joind and bring them first, also calculate stats
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = relations[predicates[i].relationA];
            uint64_t relB = relations[predicates[i].relationB];

            bool f1 = isInVector(v, relA);
            bool f2 = isInVector(v, relB);

            if ( (f1 || f2) && (rel == relA || rel == relB)) {
                // reorder the filters for the one of them
                orderedCount = reorderFilters(relA);
                orderedCount = reorderFilters(relB);

                // Stats newStats = evalJoinStats(relA, predicates[i].columnA, relB, predicates[i].columnB);
                // this->cost += newStats.f;
                this->cost += 51;
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }
    this->lastPredicate = orderedCount;

    // std::cout << "For " << predicateStr << '\n';
    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&(this->predicates[i]));
    // }
}


// Constructor for joins
JoinTree::JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo) {
    this->cost = 0;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, jt->predicates, this->predicatesCount);

    this->lastPredicate = jt->lastPredicate;
    this->lastPredicate = reorderFilters(rel);

    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     std::cout << "\t";
    //     printPredicate(&(this->predicates[i]));
    // }


    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = orderedCount; i < this->predicatesCount; i++) {
        // Find which relatios are joind and bring them first, also calculate stats
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = relations[predicates[i].relationA];
            uint64_t relB = relations[predicates[i].relationB];

            bool f1 = isInVector(jt->set, relA);
            bool f2 = isInVector(jt->set, relB);
            if ( (f1 || f2) && (rel == relA || rel == relB)) {
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }
    this->lastPredicate = orderedCount;

    this->set = makeSet(jt->set, rel);
    this->predicateStr = vectorToString(this->set);

    // std::cout << "For " << predicateStr << '\n';
    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&(this->predicates[i]));
    // }
}

uint64_t JoinTree::reorderFilters(uint64_t rel) {
    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = orderedCount; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER || predicates[i].predicateType == SELFJOIN) {
            uint64_t curRel = relations[predicates[i].relationA];
            if (curRel == rel) {
                swapPredicates(&predicates[i], &predicates[orderedCount]);
                orderedCount++;
            }
        }
    }
    this->lastPredicate = orderedCount;
    return orderedCount;
}

string JoinTree::getPredicateStr() { return predicateStr; }

double JoinTree::getCost() { return cost; }

Predicate * JoinTree::getPredicates() { return predicates; }

uint64_t JoinTree::getPredicatesCount() { return predicatesCount; }

void JoinTree::updateJoinTree(double eval) {
    this->cost = eval;
}

JoinTree::~JoinTree () {}

void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

// Check if the relation rel is in the vector
bool isInVector(std::vector<uint64_t> v, uint64_t rel) {
    for (auto it= v.begin(); it != v.end(); ++it) {
        if (*it == rel) {
            return true;
        }
    }
    return false;
}

// Conert a vector of uint64_t to string
std::string vectorToString(std::vector<uint64_t> v) {
    std::string str;
    for (auto it: v) {
        str.append(std::to_string(it));
    }
    return str;
}

// Get a vector and a relation and add the relation in the right place
std::vector<uint64_t> makeSet(std::vector<uint64_t> cur, uint64_t rel) {
    for (auto it = cur.begin(); it != cur.end(); it++) {
        if (rel < *it) {
            cur.insert(it, rel);
            return cur;
        }
    }
    cur.push_back(rel);
    return cur;
}

bool isConnected(std::vector<uint64_t> v, uint64_t rel, QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;
    uint64_t * relations = queryInfo->relations;

    for (size_t i = 0; i < count; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = relations[predicates[i].relationA];
            uint64_t relB = relations[predicates[i].relationB];

            bool f1 = isInVector(v, relA);
            bool f2 = isInVector(v, relB);

            if ( (f1 || f2) && (rel == relA || rel == relB))
                return true;
        }
    }

    return false;
}

/*
Creates a vector that contains all the posible combinations of the relations
*/
std::vector<std::vector<uint64_t>> makeRelationsSet(QueryInfo * queryInfo) {
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;

    std::vector<std::vector<uint64_t>> relationsSet;

    // initialize costs for single relations and add single relations to the set
    for (size_t i = 0; i < count; i++) {
        std::vector<uint64_t> newV = {relations[i]};
        relationsSet.push_back(newV);
    }

    size_t start = 0;
    size_t end = count;
    std::vector<uint64_t> newV;
    while (1) {
        // For every combination of sets
        std::vector<uint64_t> newV;
        for (size_t i = start; i < end; i++) {
            std::vector<uint64_t> cur = relationsSet[i];
            size_t lastRel = cur.back();
            size_t last;
            for (size_t j = 0; j < count; j++) {
                if (relations[j] == lastRel) {
                    last = j + 1;
                    break;
                }
            }
            for (size_t j = last; j < count; j++) {
                // Ignore non connected sets
                if (!isConnected(cur, relations[j], queryInfo)) {
                    continue;
                }
                newV = cur;
                newV.push_back(relations[j]);
                relationsSet.push_back(newV);
            }
        }
        if (newV.size() >= count)
            break;
        start = end;
        end = relationsSet.size();
    }

    // std::cout << "Vector:" << '\n';
    // for (auto x: relationsSet) {
    //     std::cout << '\n' << "  ";
    //     for (auto y: x) {
    //         std::cout << y;
    //     }
    // }
    // std::cout << '\n';
    // std::cout << '\n';

    return relationsSet;
}

/*
Find the best order to perform the predicates
*/
void joinEnumeration(QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t predicatesCount = queryInfo->predicatesCount;

    std::vector<std::vector<uint64_t>> relationsSet = makeRelationsSet(queryInfo);

    // calculate stats for filters and self joins
    for (size_t i = 0; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER) {
            if (predicates[i].op == '=') {
                updateEqualFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            } else if (predicates[i].op == '<') {
                updateLessFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            } else {
                updateGreaterFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            }
        } else if (predicates[i].predicateType == SELFJOIN) {
            updateSelfJoinStats(predicates[i].relationA, predicates[i].columnA, predicates[i].columnB);
        }
    }

    std::unordered_map<std::string, JoinTree *> bestTree;
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;

    // Add to the has table the costs for the single relations
    for (size_t i = 0; i < count; i++) {
        std::string relStr = std::to_string(relations[i]);
        std::vector<uint64_t> v = { relations[i] };
        JoinTree * jt = new JoinTree(relations[i], queryInfo);
        bestTree[relStr] = jt;
    }

    string str = "";
    // uint64_t start = 0;
    // uint64_t end = count;
    // Add to the hash table the costs for all the other combinations
    for (auto cur: relationsSet) {
        // For each subset of relations find the cost if it's joined wit each
        // relation
        for (size_t i = 0; i < count; i++) {
            uint64_t rel = relations[i];
            bool found = isInVector(cur, rel);
            // Avoid cross products and don't perform any action if they are
            // disconnected
            if (found || !isConnected(cur, rel, queryInfo)) {
                continue;
            }
            std::string curStr = vectorToString(cur);
            // std::cout << "curStr = " << curStr << '\n';
            std::vector<uint64_t> set = makeSet(cur, rel);
            std::string str = vectorToString(set);
            JoinTree * curTree = bestTree[curStr];
            // std::cout << curStr << " and " << rel << " are connected, so set = " << str << '\n';
            if (curTree == NULL) {
                // std::cout << "--No tree for curTree (" << curStr << ")" << '\n';
                JoinTree * jt = new JoinTree(cur, rel, queryInfo);
                bestTree[curStr] = jt;
            } else {
                JoinTree * best = bestTree[str];
                // std::cout << "--Tree for curTree (" << curStr << ")" << '\n';
                if (best == NULL) {
                    // std::cout << "\tNo tree for " << str << '\n';
                    JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                    bestTree[str] = jt;
                } else {
                    // std::cout << "\tTree for " << str << '\n';

                    // In case we have a tree with a set of 2 relations it means
                    // we just check for the reverse set. For example if we
                    // have 01 the, second time we check for 10. We don't need
                    // to do anything in this case
                    if (str.length() == 2) {
                        // std::cout << "Lenght 2, not doing anything" << '\n';
                        continue;
                    }
                    // eval cost and check if it's better than the cost of best
                    // JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                    // bestTree[str] = jt;

                }
            }
        }
    }

    // std::cout << "Hash:" << '\n';
    // for (auto x = bestTree.begin(); x!=  bestTree.end(); x++) {
    //     std::cout << x->first << ":  " << x->second->getPredicateStr() << ", " << x->second->getCost() << '\n';
    // }
}
