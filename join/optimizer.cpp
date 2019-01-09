#include "optimizer.hpp"
#include "predicates.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include "stats.hpp"
#include <unordered_map>

using std::string;

extern Stats ** stats;


PredicateNode::PredicateNode (std::string a, uint64_t b) {
    this->predicateStr = a + std::to_string(b);
}

PredicateNode::PredicateNode (uint64_t rel, QueryInfo * queryInfo) {
    this->predicateStr = std::to_string(rel);

    this->predicates = queryInfo->predicates;
    this->predicatesCount = queryInfo->predicatesCount;
}

PredicateNode::PredicateNode (PredicateNode * n, uint64_t rel, QueryInfo * queryInfo) {
    this->predicateStr = "";
    this->predicates = n->predicates;
    this->predicatesCount = queryInfo->predicatesCount;

    this->lastPredicate = reorderFilters(rel);

}


void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

uint64_t PredicateNode::reorderFilters(uint64_t rel) {
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


string PredicateNode::getPredicateStr() { return predicateStr; }

double PredicateNode::getCost() { return cost; }

PredicateNode::~PredicateNode () {}


bool isConected(string str, uint64_t rel, QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;

    for (size_t i = 0; i < count; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = predicates[i].relationA;
            uint64_t relB = predicates[i].relationB;
            size_t f1 = str.find(std::to_string(relA));
            size_t f2 = str.find(std::to_string(relB));
            if ( (f1!=string::npos || f2!=string::npos) && (rel == relA || rel == relB)) {
                joinStats(relA, predicates[i].columnA, relB, predicates[i].columnB);
                return true;
            }
        }
    }

    return false;
}


void joinEnumeration(QueryInfo* queryInfo) {
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;
    Predicate * predicates = queryInfo->predicates;
    uint64_t predicatesCount = queryInfo->predicatesCount;

    // calculate stats for filters and self joins
    for (size_t i = 0; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER) {
            if (predicates[i].op == '=') {
                equalFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            } else if (predicates[i].op == '<') {
                lessFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            } else {
                greaterFilterStats(predicates[i].relationA, \
                        predicates[i].columnA, predicates[i].value);
            }
        } else if (predicates[i].predicateType == SELFJOIN) {
            selfJoinStats(predicates[i].relationA, predicates[i].columnA, predicates[i].columnB);
        }
    }

    std::vector<string> relationsSet;
    std::unordered_map<string, PredicateNode *> bestTree;

    string str;
    // initialize costs for single relations and add single relations to the set
    for (size_t i = 0; i < count; i++) {
        str = std::to_string(relations[i]);
        PredicateNode * n = new PredicateNode(relations[i], queryInfo);
        relationsSet.push_back(std::to_string(relations[i]));
        bestTree[str] = n;
    }


    size_t start = 0;
    size_t end = count;
    while (1) {
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
                str = relationsSet[i] + jStr;
                relationsSet.push_back(str);
                if (isConected(relationsSet[i], relations[j], queryInfo)) {
                    // PredicateNode * n = new PredicateNode(relationsSet[i], relations[j], queryInfo);
                    // bestTree[str] = n;
                }
            }
        }
        if (str.length() >= count)
            break;
        start = end;
        end = relationsSet.size();
    }

    for (size_t i = 0; i < count; i++) {
        printStats(relations[i]);
        std::cout << '\n';
    }
}
