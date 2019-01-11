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

JoinTree::JoinTree (std::vector<uint64_t> v, uint64_t rel, QueryInfo * queryInfo) {
    this->set = makeSet(v, rel);
    this->predicateStr = vectorToString(set);
    this->cost = 500;
}


JoinTree::JoinTree (uint64_t rel, QueryInfo * queryInfo) {
    this->set.push_back(rel);
    this->predicateStr = std::to_string(rel);
    this->cost = stats[rel][0].d;
    this->set = {rel};
    this->lastPredicate = 0;

    this->predicates = queryInfo->predicates;
    this->predicatesCount = queryInfo->predicatesCount;

    this->lastPredicate = reorderFilters(rel);
}

JoinTree::JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo) {
    this->predicateStr = jt->getPredicateStr() + std::to_string(rel);
    this->predicates = jt->predicates;
    this->predicatesCount = queryInfo->predicatesCount;
    this->cost = jt->cost;

    // if (n->predicateStr.length() != 1) {
    //     this->lastPredicate = n->lastPredicate;
    //     reorderFilters(atoi(&(n->predicateStr[0])));
    // }
    this->lastPredicate = reorderFilters(rel);

    uint64_t orderedCount = this->lastPredicate;
    std::string str = jt->predicateStr;
    for (size_t i = 0; i < this->predicatesCount; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = predicates[i].relationA;
            uint64_t relB = predicates[i].relationB;
            size_t f1 = str.find(std::to_string(relA));
            size_t f2 = str.find(std::to_string(relB));
            if ( (f1!=string::npos || f2!=string::npos) && (rel == relA || rel == relB)) {
                Stats newStats = evalJoinStats(relA, predicates[i].columnA, relB, predicates[i].columnB);
                this->cost += newStats.f;
                // reorderJoins(relA, relB);
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }

}

uint64_t JoinTree::reorderFilters(uint64_t rel) {
    // Predicate * predicates = this->predicates;
    // uint64_t count = queryInfo->predicatesCount;

    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = 0; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER || predicates[i].predicateType == SELFJOIN) {
            if (predicates[i].relationA == rel) {
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

bool isInVector(std::vector<uint64_t> v, uint64_t rel) {
    for (auto it: v) {
        if (it == rel) {
            return true;
        }
    }
    return false;
}

void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

bool isConnected(std::vector<uint64_t> v, uint64_t rel, QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;

    for (size_t i = 0; i < count; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = predicates[i].relationA;
            uint64_t relB = predicates[i].relationB;
            bool f1 = isInVector(v, relA);
            bool f2 = isInVector(v, relB);
            if ( (f1 || f2) && (rel == relA || rel == relB)) {
                // Stats eval = evalJoinStats(relA, predicates[i].columnA, relB, predicates[i].columnB);
                // return eval.f;
                return true;
            }
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

    return relationsSet;
}

// Check if the relation rel is in the vector
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

std::string vectorToString(std::vector<uint64_t> v) {
    std::string str;
    for (auto it: v) {
        str.append(std::to_string(it));
    }
    return str;
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
            // std::cout << " and " << rel << " are connected" << '\n';
            std::string curStr = vectorToString(cur);
            std::cout << "curStr = " << curStr << '\n';
            std::vector<uint64_t> set = makeSet(cur, rel);
            std::string str = vectorToString(set);
            // std::cout << "Set = " << str << '\n';
            JoinTree * best = bestTree[str];
            if (best == NULL) {
                std::cout << "No tree for " << str << '\n';
                // JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                // bestTree[str] = jt;
            } else {
                std::cout << "Tree for " << str << '\n';
                // eval cost and check if it's better than the cost of best
            }
        }
    }

    // std::cout << "Hash:" << '\n';
    // for (auto x = bestTree.begin(); x!=  bestTree.end(); x++) {
    //     std::cout << x->first << ":  " << x->second->getPredicateStr() << ", " << x->second->getCost() << '\n';
    // }
}
