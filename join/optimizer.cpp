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


JoinTree::JoinTree (std::string a, uint64_t b) {
    this->predicateStr = a + std::to_string(b);
    this->cost = -12;
}

JoinTree::JoinTree (uint64_t rel, QueryInfo * queryInfo) {
    this->predicateStr = std::to_string(rel);
    this->cost = -300;

    this->predicates = queryInfo->predicates;
    this->predicatesCount = queryInfo->predicatesCount;
}

JoinTree::JoinTree (JoinTree * n, uint64_t rel, QueryInfo * queryInfo) {
    this->predicateStr = "";
    this->predicates = n->predicates;
    this->predicatesCount = queryInfo->predicatesCount;
    this->cost = 500;

    if (n->predicateStr.length() != 1) {
        this->lastPredicate = n->lastPredicate;
        reorderFilters(atoi(&(n->predicateStr[0])));
    }
    this->lastPredicate = reorderFilters(rel);

    uint64_t orderedCount = 0;
    std::string str = n->predicateStr;
    for (size_t i = 0; i < this->predicatesCount; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = predicates[i].relationA;
            uint64_t relB = predicates[i].relationB;
            size_t f1 = str.find(std::to_string(relA));
            size_t f2 = str.find(std::to_string(relB));
            if ( (f1!=string::npos || f2!=string::npos) && (rel == relA || rel == relB)) {
                Stats newStats = evalJoinStats(relA, predicates[i].columnA, relB, predicates[i].columnB);
                this->cost = newStats.f;
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

    uint64_t orderedCount = 0;
    for (size_t i = 0; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER || predicates[i].predicateType == SELFJOIN) {
            if (predicates[i].relationA == rel) {
                swapPredicates(&predicates[i], &predicates[orderedCount]);
                orderedCount++;
            }
        }
    }
    this->predicatesCount = orderedCount;
    return orderedCount;
}

string JoinTree::getPredicateStr() { return predicateStr; }

double JoinTree::getCost() { return cost; }

void JoinTree::updateJoinTree(double eval) {
    this->cost = eval;
}

JoinTree::~JoinTree () {}



void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

bool isConnected(string str, uint64_t rel, QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;

    for (size_t i = 0; i < count; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = predicates[i].relationA;
            uint64_t relB = predicates[i].relationB;
            size_t f1 = str.find(std::to_string(relA));
            size_t f2 = str.find(std::to_string(relB));
            if ( (f1!=string::npos || f2!=string::npos) && (rel == relA || rel == relB)) {
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
std::vector<string> makeRelationsSet(QueryInfo * queryInfo) {
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;

    std::vector<string> relationsSet;

    string str;
    // initialize costs for single relations and add single relations to the set
    for (size_t i = 0; i < count; i++) {
        str = std::to_string(relations[i]);
        relationsSet.push_back(std::to_string(relations[i]));
    }

    size_t start = 0;
    size_t end = count;
    while (1) {
        // For every combination of sets
        for (size_t i = start; i < end; i++) {
            string cur = relationsSet[i];
            size_t lastRel = atoi(&cur.back());
            size_t last;
            for (size_t j = 0; j < count; j++) {
                if (relations[j] == lastRel) {
                    last = j + 1;
                    break;
                }
            }
            for (size_t j = last; j < count; j++) {
                string jStr = std::to_string(relations[j]);
                str = cur + jStr;
                relationsSet.push_back(str);
            }
        }
        if (str.length() >= count)
            break;
        start = end;
        end = relationsSet.size();
    }

    std::cout << "Vector:" << '\n';
    for (auto x: relationsSet) {
        std::cout << "  " << x << '\n';
    }

    return relationsSet;
}

/*
Find the best order to perform the predicates
*/
void joinEnumeration(QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t predicatesCount = queryInfo->predicatesCount;

    std::vector<string> relationsSet = makeRelationsSet(queryInfo);

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

    std::unordered_map<string, JoinTree *> bestTree;
    std::unordered_set<JoinTree *> bestTree2;
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;

    std::cout << "count = " << count << '\n';
    // Add to the has table the cotst for the single relations
    for (size_t i = 0; i < count; i++) {
        JoinTree * n = new JoinTree(relations[i], queryInfo);
        bestTree[std::to_string(relations[i])] = n;
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
            size_t found = cur.find(std::to_string(rel));
            // Avoid cross products and don't perform any action if they are
            // disconnected
            if (found != string::npos || !isConnected(cur, rel, queryInfo)) {
                continue;
            }
            std::cout << cur << " and " << rel << " are connected" << '\n';

        }
    }

    std::cout << "Hash:" << '\n';
    for (auto x = bestTree.begin(); x!=  bestTree.end(); x++) {
        std::cout << x->first << ":  " << x->second->getPredicateStr() << ", " << x->second->getCost() << '\n';
    }
}
