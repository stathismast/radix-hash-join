#include "optimizer.hpp"
#include "predicates.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include "stats.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using std::string;

extern Relation * r;
extern uint64_t relationsSize;
extern Stats ** stats;

// constructor for the single relation nodes
JoinTree::JoinTree (uint64_t rel, QueryInfo * queryInfo) {
    // std::cerr << "===================================" << '\n';
    this->cost = 0;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, queryInfo->predicates, this->predicatesCount);

    this->predicateStr = std::to_string(rel);
    // A set of relations that would represent all our relations in our current
    // intermediate result
    this->irSet = {rel};

    // After reordering the predicates keep the position of the last ordered
    // predicate. It's 0 since no predicates have been ordered yet
    this->lastPredicate = 0;
    // Bring all the filters and self joins for the current relation first
    this->lastPredicate = reorderFilters(rel);

    this->myStats = copyStats(this->myStats, stats, queryInfo);
    // std::cout << "Made " << predicateStr << '\n';
    // std::cout << "lastPredicate = " << lastPredicate << '\n';
    // for (size_t i = 0; i < predicatesCount; i++) {
    //     printPredicate(&predicates[i]);
    // }
    // std::cout << '\n';
    // printStats();
    // std::cerr << "===================================\n" << '\n';
}

// Constructor for joins
JoinTree::JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo) {
    // std::cerr << "===================================" << '\n';
    this->cost = jt->cost;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    // We copy the predicates into a new array since each JoinTree will have
    // different order for them
    copyPredicates(&this->predicates, jt->predicates, this->predicatesCount);
    // Thre predicates of jt are already ordered for all the relations in
    // the jt and we will then work for all the joins between rel and the
    // relations in jt
    this->lastPredicate = jt->lastPredicate;
    // Just put the one filter we will have first. Does not work with multiple
    // filters but our imolementation does not support this yet
    // this->lastPredicate = reorderFilters();
    // this->lastPredicate = reorderFilters(rel);
    // this->cost += myStats[relations[rel]][0].f;
    // std::cout << "Joining " << jt->predicateStr << " and " << rel << '\n';

    // Assing jt set to our set temporarily so in the calculation of the stats
    // this set will be used
    this->irSet = jt->irSet;
    // We start with the stast of jt since some joins and filters have been done
    this->myStats = copyStats(this->myStats, jt->myStats, queryInfo);
    // this->lastPredicate = reorderFilters(rel);
    // this->cost += myStats[relations[rel]][0].f;

    uint64_t orderedCount = this->lastPredicate;
    // Find which relations are joind and bring them first, also calculate stats
    for (size_t i = orderedCount; i < this->predicatesCount; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relPosA = predicates[i].relationA;
            uint64_t relPosB = predicates[i].relationB;
            uint64_t relA = relations[relPosA];
            uint64_t relB = relations[relPosB];
            uint64_t colA = predicates[i].columnA;
            uint64_t colB = predicates[i].columnB;
            // Find if any of the relations of current join are in set
            bool f1 = isInVector(this->irSet, relPosA);
            bool f2 = isInVector(this->irSet, relPosB);
            // std::cout << "relA = " << relA << " relB = " << relB << " rel = " << rel << '\n';
            if ( (f1 || f2) && (rel == relPosA || rel == relPosB)) {
                fflush(stdout);
                // The first relation will be the one on this->irSet
                if (f1 == true) {
                    updateJoinStats(relA, colA, relB, colB);
                } else if (f2 == true) {
                    updateJoinStats(relB, colB, relA, colA);
                }
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }
    this->lastPredicate = orderedCount;
    // this->cost += myStats[rel][0].f;        // update the cost
    this->cost += myStats[relations[rel]][0].f;        // update the cost

    // Now make the final irSet where all relations of jt plus rel will be
    // contained
    this->irSet = makeSet(jt->irSet, rel);
    this->predicateStr = vectorToString(this->irSet);
    // this->lastPredicate = reorderFilters(rel);

    // std::cout << "Made " << predicateStr << '\n';
    // std::cout << "Connected " << jt->predicateStr << " and " << rel << '\n';
    // std::cout << "Cost for " << predicateStr << " = " << jt->cost << " + " << myStats[relations[rel]][0].f << '\n';
    // for (size_t i = 0; i < predicatesCount; i++) {
    //     printPredicate(&predicates[i]);
    // }
    // std::cerr << "===================================\n" << '\n';
}

void JoinTree::updateStats(uint64_t rel, uint64_t col, Stats newStats){
    myStats[rel][col].l = newStats.l;
    myStats[rel][col].u = newStats.u;
    myStats[rel][col].f = newStats.f;
    myStats[rel][col].d = newStats.d;
}

void JoinTree::updateLessFilterStatsIR(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    newStats.l = myStats[rel][col].l;
    if (k > u) {
        k = u;
    }
    newStats.u = k;
    if (k < l) {
        // if filter value is lower than the lowest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(k-l))/(u-l);
        newStats.f = (f*(k-l))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = myStats[rel][i].d;
        double fc = myStats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        }
        else if(f == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            // std::cout << 1-(newStats.f/f) << std::endl;
            myStats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            myStats[rel][i].f = newStats.f;
        }
    }

    // Update the stats of every other column that would be in the intermediate
    for (uint64_t j = 0; j < this->irSet.size(); j++) {
        // std::cout << "it = " << set[i] << '\n';
        uint64_t setRel = relations[this->irSet[j]];
        if (setRel != rel ) {
            for(uint64_t i=0; i<r[setRel].cols; i++){

                double dc = myStats[setRel][i].d;
                double fc = myStats[setRel][i].f;
                // Check if dc or f are equal to 0 and act accoridingly
                // This way we can avoid dividing by 0
                if(dc == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                }
                else if(f == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                } else {
                    // std::cout << fc/dc << std::endl;
                    // std::cout << 1-(newStats.f/f) << std::endl;
                    myStats[setRel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
                    myStats[setRel][i].f = newStats.f;
                }
            }
        }
    }
}

void JoinTree::updateGreaterFilterStatsIR(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    if (k < l) {
        k = l;
    }
    newStats.l = k;
    newStats.u = myStats[rel][col].u;
    if (k > u) {
        // if filter value is higher than the highest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(u-k))/(u-l);
        newStats.f = (f*(u-k))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = myStats[rel][i].d;
        double fc = myStats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        }
        else if(f == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            // std::cout << 1-(newStats.f/f) << std::endl;
            myStats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            myStats[rel][i].f = newStats.f;
        }
    }

    // Update the stats of every other column that would be in the intermediate
    for (uint64_t j = 0; j < this->irSet.size(); j++) {
        // std::cout << "it = " << set[i] << '\n';
        uint64_t setRel = relations[this->irSet[j]];
        if (setRel != rel ) {
            for(uint64_t i=0; i<r[setRel].cols; i++){
                double dc = myStats[setRel][i].d;
                double fc = myStats[setRel][i].f;
                // Check if dc or f are equal to 0 and act accoridingly
                // This way we can avoid dividing by 0
                if(dc == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                }
                else if(f == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                } else {
                    // std::cout << fc/dc << std::endl;
                    // std::cout << 1-(newStats.f/f) << std::endl;
                    myStats[setRel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
                    myStats[setRel][i].f = newStats.f;
                }
            }
        }
    }
}


void JoinTree::updateLessFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    newStats.l = myStats[rel][col].l;
    if (k > u) {
        k = u;
    }
    newStats.u = k;
    if (k < l) {
        // if filter value is lower than the lowest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(k-l))/(u-l);
        newStats.f = (f*(k-l))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = myStats[rel][i].d;
        double fc = myStats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        }
        else if(f == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            // std::cout << 1-(newStats.f/f) << std::endl;
            myStats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            myStats[rel][i].f = newStats.f;
        }
    }
}

void JoinTree::updateGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    if (k < l) {
        k = l;
    }
    newStats.l = k;
    newStats.u = myStats[rel][col].u;
    if (k > u) {
        // if filter value is higher than the highest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(u-k))/(u-l);
        newStats.f = (f*(u-k))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = myStats[rel][i].d;
        double fc = myStats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        }
        else if(f == 0){
            myStats[rel][i].d = 0;
            myStats[rel][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            // std::cout << 1-(newStats.f/f) << std::endl;
            myStats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            myStats[rel][i].f = newStats.f;
        }
    }
}


void JoinTree::updateJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB) {
    Stats newStatsA;
    Stats newStatsB;
    double da = myStats[relA][colA].d;
    double db = myStats[relB][colB].d;
    // std::cout << "relA = " << relA << ", relB = " << relB << '\n';
    // std::cout << "colA = " << colA << ", colB = " << colB << '\n';

    double newL = max(myStats[relA][colA].l, myStats[relB][colB].l);
    double newU = min(myStats[relA][colA].u, myStats[relB][colB].u);
    // Use filter in each column so they will have same lower and upper value
    // and all other stats will be upadated accordingly
    updateGreaterFilterStatsIR(relA, colA, newL);
    updateLessFilterStatsIR(relA, colA, newU);
    updateGreaterFilterStats(relB, colB, newL);
    updateLessFilterStats(relB, colB, newU);

    newStatsA.l = newStatsB.l = newL;
    newStatsA.u = newStatsB.u = newU;
    double n = newU - newL + 1;
    // std::cout << "n = " << n << '\n';
    if (n == 0) {
        newStatsA.f = newStatsA.d = newStatsB.f = newStatsB.d = 0;
    }
    else {
        newStatsA.f = newStatsB.f = (myStats[relA][colA].f*myStats[relB][colB].f)/n;
        newStatsA.d = newStatsB.d = (myStats[relA][colA].d*myStats[relB][colB].d)/n;
    }

    updateStats(relA,colA,newStatsA);
    updateStats(relB,colB,newStatsB);

    // Update the stats of every other column of the first relation
    for(uint64_t i=0; i<r[relA].cols; i++){
        if(i == colA) continue;

        double dc = myStats[relA][i].d;
        double fc = myStats[relA][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[relA][i].d = 0;
            myStats[relA][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            myStats[relA][i].d = dc * (1-pow((1-(newStatsA.d/da)), fc/dc));
            myStats[relA][i].f = newStatsA.f;
        }
    }

    // Update the stast of every other column of the second relation
    for(uint64_t i=0; i<r[relB].cols; i++){
        if(i == colB) continue;

        double dc = myStats[relB][i].d;
        double fc = myStats[relB][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            myStats[relB][i].d = 0;
            myStats[relB][i].f = 0;
        } else {
            // std::cout << fc/dc << std::endl;
            myStats[relB][i].d = dc * (1-pow((1-(newStatsB.d/db)), fc/dc));
            myStats[relB][i].f = newStatsB.f;
        }
    }

    // Update the stats of every other column that would be in the intermediate
    for (uint64_t j = 0; j < this->irSet.size(); j++) {
        // std::cout << "it = " << set[j] << '\n';
        uint64_t setRel = relations[this->irSet[j]];
        if (setRel != relB && setRel != relA) {
            for(uint64_t i=0; i<r[setRel].cols; i++){

                double dc = myStats[setRel][i].d;
                double fc = myStats[setRel][i].f;
                // Check if dc or f are equal to 0 and act accoridingly
                // This way we can avoid dividing by 0
                if(dc == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                }
                else if(newStatsA.f == 0){
                    myStats[setRel][i].d = 0;
                    myStats[setRel][i].f = 0;
                } else {
                    // std::cout << fc/dc << std::endl;
                    // std::cout << 1-(newStats.f/f) << std::endl;
                    myStats[setRel][i].d = dc * (1-pow((1-(newStatsA.d/da)), fc/dc));
                    myStats[setRel][i].f = newStatsA.f;
                }
            }
        }
    }
}

Stats JoinTree::evalLessFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    newStats.l = myStats[rel][col].l;
    if (k > u) {
        k = u;
    }
    newStats.u = k;
    if (k < l) {
        // if filter value is lower than the lowest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(k-l))/(u-l);
        newStats.f = (f*(k-l))/(u-l);
    }

    return newStats;
}

Stats JoinTree::evalGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
    double l = myStats[rel][col].l;
    double u = myStats[rel][col].u;
    double f = myStats[rel][col].f;
    double d = myStats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    if (k < l) {
        k = l;
    }
    newStats.l = k;
    newStats.u = myStats[rel][col].u;
    if (k > u) {
        // if filter value is higher than the highest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (myStats[rel][col].l == myStats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = (d*(u-k))/(u-l);
        newStats.f = (f*(u-k))/(u-l);
    }

    return newStats;
}

Stats JoinTree::evalJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB) {
    Stats newStatsA;
    Stats newStatsB;

    double newL = max(myStats[relA][colA].l, myStats[relB][colB].l);
    double newU = min(myStats[relA][colA].u, myStats[relB][colB].u);
    // Use filter in each column so they will have same lower and upper value
    // and all other myStats will be upadated accordingly
    newStatsA = evalGreaterFilterStats(relA, colA, newL);
    // evalLessFilterStats(relA, colA, newU);
    if (newU < newStatsA.l) {
        newStatsA.d = 0;
    }
    else if (newStatsA.l == newStatsA.u) {
        newStatsA.d = 0;
    } else {
        newStatsA.d = (newStatsA.d*(newU - newStatsA.l))/(newStatsA.u - newStatsA.l);
    }

    newStatsB = evalGreaterFilterStats(relB, colB, newL);
    // evalLessFilterStats(relB, colB, newU);
    if (newU < newStatsB.l) {
        newStatsB.f = 0;
    }
    else if (newStatsB.l == newStatsB.u) {
        newStatsB.f = 0;
    } else {
        newStatsB.f = (newStatsB.f*(newU - newStatsB.l))/(newStatsB.u - newStatsB.l);
    }

    newStatsA.l = newStatsB.l = newL;
    newStatsA.u = newStatsB.u = newU;
    double n = newU - newL + 1;
    // std::cout << "n = " << n << '\n';
    if (n == 0) {
        newStatsA.f = 0;
    }
    else {
        newStatsA.f = (newStatsA.f*newStatsB.f)/n;
    }

    return newStatsA;
}

uint64_t JoinTree::reorderFilters() {
    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = orderedCount; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER || predicates[i].predicateType == SELFJOIN) {
            swapPredicates(&predicates[i], &predicates[orderedCount]);
            orderedCount++;
        }
    }
    this->lastPredicate = orderedCount;
    return this->lastPredicate;
}

uint64_t JoinTree::reorderFilters(uint64_t rel) {
    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = orderedCount; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER || predicates[i].predicateType == SELFJOIN) {
            uint64_t curRel = predicates[i].relationA;
            // uint64_t curRel = relations[predicates[i].relationA];
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

void JoinTree::printStats() {
    for (size_t j = 0; j < relationsCount; j++) {
        uint64_t rel = relations[j];
        for (size_t i = 0; i < r[rel].cols; i++) {
            std::cout << rel << "." << i;
            fflush(stdout);
            std::cout << ": l=" << myStats[rel][i].l
            << "  u=" << myStats[rel][i].u
            << "  f=" << myStats[rel][i].f
            << "  d=" << myStats[rel][i].d << "\n";
        }
        std::cout << '\n';
    }
}

JoinTree::~JoinTree () {
    // TO IMPLEMENT THIS EVENTUALY
    // delete[] relations;
    delete[] predicates;
    for(uint64_t i=0; i<relationsSize; i++){
        delete[] myStats[i];
    }
    delete[] myStats;
}

void swapPredicates(Predicate * A, Predicate * B){
    Predicate temp = *A;
    *A = *B;
    *B = temp;
}

// Check if the relation rel is in the vector
bool isInVector(std::vector<uint64_t> v, uint64_t rel) {
    for (auto it= v.begin(); it != v.end(); ++it) {
        if (*it == rel)
            return true;
    }
    return false;
}

// Conert a vector of uint64_t to string
std::string vectorToString(std::vector<uint64_t> v) {
    std::string str;
    bool first = true;
    for (auto it: v) {
        // str += "-" + std::to_string(it);
        if (first)
            first = false;
        else
            str.append("-");
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

// Find if a set of relations has a join with another relation
bool isConnected(std::vector<uint64_t> v, uint64_t relPos, QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t count = queryInfo->predicatesCount;
    // uint64_t * relations = queryInfo->relations;

    // uint64_t rel = relations[relPos];
    // std::cout << "Checking if realtion " << relPos << " is connectd with " << vectorToString(v) << '\n';
    for (size_t i = 0; i < count; i++) {
        if (predicates[i].predicateType == JOIN) {
            uint64_t relPosA = predicates[i].relationA;
            uint64_t relPosB = predicates[i].relationB;
            // uint64_t relA = relations[predicates[i].relationA];
            // uint64_t relB = relations[predicates[i].relationB];

            bool f1 = isInVector(v, relPosA);
            if (f1) {
                // std::cout << "A:" << i << " in " << vectorToString(v) << '\n';
            }
            bool f2 = isInVector(v, relPosB);
            if (f2) {
                // std::cout << "B:" << i << " in " << vectorToString(v) << '\n';
            }
            // std::cout << "relPosA = " << relPosA << ", relPosB = " << relPosB << '\n';
            if ( (f1 || f2) && (relPos == relPosA || relPos == relPosB))
                return true;
        }
    }

    return false;
}


/*
Creates a vector that contains all the posible combinations of the relations
*/
std::vector<std::vector<uint64_t>> makeRelationsSet(QueryInfo * queryInfo, uint64_t filterRelPos) {
    uint64_t count = queryInfo->relationsCount;
    uint64_t * relations = new uint64_t[count];
    for (size_t i = 0; i < count; i++) {
        relations[i] = queryInfo->relations[i];
    }
    // std::cout << "count = " << count << '\n';
    // quickSort(relations, 0, count - 1);
    // for (size_t i = 0; i < count; i++) {
    //     // std::cout << "ordered set = " << relations[i] << '\n';
    // }
    std::vector<std::vector<uint64_t>> relationsSet;
    // We use 14 here since we know we have at most 4 relations and all the
    // combinations can be at most combinations, regardless the order
    // relationsSet.reserve(14);

    // initialize the vector for single relations
    for (size_t i = 0; i < count; i++) {
        // std::vector<uint64_t> newV = {relations[i]};
        // relationsSet.push_back(newV);
        std::vector<uint64_t> newV = {i};
        relationsSet.push_back(newV);
    }

    // for (size_t i = 0; i < count; i++) {
    //     std::cout << "set = " << vectorToString(relationsSet[i]) << '\n';
    // }

    size_t start = 0;
    size_t end = count;
    std::vector<uint64_t> newV;
    while (1) {
        // For every combination of sets
        // std::cout << start << ", " << end << '\n';
        std::vector<uint64_t> newV;
        for (size_t i = start; i < end; i++) {
            std::vector<uint64_t> cur = relationsSet[i];
            std::string curStr = vectorToString(cur);
            size_t lastRel = cur.back();
            size_t last = 0;
            // Start always from the next of current next since combinations with
            // previous ones have already been found
            for (size_t j = 0; j < count; j++) {
                if (j == lastRel) {
                    last = j + 1;
                    break;
                }
            }
            for (size_t j = last; j < count; j++) {
                // std::cout << "i = " << i << " last = " << last << " and count = " << count  << '\n';
                // Ignore non connected sets
                // std::cout << "cur = " << curStr << " and rel = " << j << '\n';
                if (!isConnected(cur, j, queryInfo) || isInVector(cur, j)) {
                    // std::cout << " are not connected" << "\n\n";
                    continue;
                }
                newV = makeSet(cur, j);
                // newV = cur;
                // newV.push_back(relations[j]);
                // sort(newV.begin(), newV.end());
                relationsSet.push_back(newV);
                // std::cout << "pushed " << vectorToString(newV) << '\n';
                // std::cout << " are connected" << "\n\n";
            }
            // std::cout << "start = " << start << " end = " << end << '\n';
        }
        // Stop when the last set that got added contained all the relations
        if (newV.size() >= count)
            break;
        start = end;
        end = relationsSet.size();
    }
    // std::cout << "\n" << '\n';
    std::vector<std::vector<uint64_t>> finalRelationsSet;
    if (filterRelPos != (uint64_t) -1) {
        for (auto it: relationsSet) {
            if (isInVector(it, filterRelPos)) {
                finalRelationsSet.push_back(it);
            }
        }
    }
    delete [] relations;
    return finalRelationsSet;
}

/*
Find the best order to perform the predicates
*/
void joinEnumeration(QueryInfo* queryInfo) {
    Predicate * predicates = queryInfo->predicates;
    uint64_t predicatesCount = queryInfo->predicatesCount;
    uint64_t * relations = queryInfo->relations;
    uint64_t count = queryInfo->relationsCount;

    // calculate stats for filters and self joins
    for (size_t i = 0; i < predicatesCount; i++) {
        uint64_t relA = relations[predicates[i].relationA];
        if (predicates[i].predicateType == FILTER) {
            // std::cout << "updating i = " << i << ", relA = " << predicates[i].relationA
            //     << "." << predicates[i].columnA<< '\n';
            if (predicates[i].op == '=') {
                updateEqualFilterStats(relA, \
                        predicates[i].columnA, predicates[i].value);
            } else if (predicates[i].op == '<') {
                updateLessFilterStats(relA, \
                        predicates[i].columnA, predicates[i].value);
            } else {
                updateGreaterFilterStats(relA, \
                        predicates[i].columnA, predicates[i].value);
            }
        } else if (predicates[i].predicateType == SELFJOIN) {
            updateSelfJoinStats(relA, predicates[i].columnA, predicates[i].columnB);
        }
    }

    // Put the filter alwasy first
    uint64_t filterRelPos = (uint64_t) -1;
    for (size_t i = 0; i < predicatesCount; i++) {
        if (predicates[i].predicateType == FILTER) {
            filterRelPos = predicates[i].relationA;
            swapPredicates(&predicates[0], &predicates[i]);
        }
    }
    // if (filterRelPos != (uint64_t) -1) {
    //     std::cout << "Filter exists for " << filterRelPos << '\n';
    //     std::string relStr = std::to_string(filterRelPos);
    //     std::vector<uint64_t> v = { filterRelPos };
    //     JoinTree * jt = new JoinTree(filterRelPos, queryInfo);
    //     bestTree[relStr] = jt;
    // }
    std::vector<std::vector<uint64_t>> relationsSet = makeRelationsSet(queryInfo, filterRelPos);
    // for (auto cur: relationsSet) {
    //     // std::cout << "set = " << vectorToString(cur) << '\n';
    // }

    std::unordered_map<std::string, JoinTree *> bestTree;

    // Add to the hash table the costs for the single relations
    for (size_t i = 0; i < count; i++) {
        uint64_t rel = i;
        std::string relStr = std::to_string(rel);
        std::vector<uint64_t> v = { rel };
        JoinTree * jt = new JoinTree(rel, queryInfo);
        bestTree[relStr] = jt;
    }


    string str = "";
    // Add to the hash table the costs for all the other combinations
    for (auto cur: relationsSet) {
        // For each subset of relations find the cost if it's joined with each
        // other single relation
        for (size_t i = 0; i < count; i++) {
            // uint64_t rel = relations[i];
            uint64_t rel = i;
            bool found = isInVector(cur, rel);
            // Avoid cross products and don't perform any action if they are
            // disconnected
            if (found || !isConnected(cur, rel, queryInfo)) {
                continue;
            }

            std::string curStr = vectorToString(cur);
            JoinTree * curTree = bestTree[curStr];

            std::vector<uint64_t> set = makeSet(cur, rel);
            std::string str = vectorToString(set);
            JoinTree * best = bestTree[str];

            if (curTree == NULL) {
                // std::cout << "For some unexpected reason curTree is null for " << curStr << '\n';
            }
            // If we have not made joint tree for the current set make it
            if (best == NULL) {
                // std::cout << "  Best does not exist for " << str << '\n';
                JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                bestTree[str] = jt;
            } else {
                // We already have a tree for the set but different join order
                // so need to find the cost of the new join order and keep the best

                // In case we have a tree with a set of 2 relations it means
                // we just check for the reverse set. For example if we
                // have 01 the, second time we check for 10. We don't need
                // to do anything in this case
                // std::cout << "  Best exists for " << str << " and we are gonna check"
                // << " if the new cost is better" << '\n';

                if (cur.size() == 1) {
                    // std::cout << "    Actualy it's a case of a set of size 2 wealredy have so we move on" << '\n';
                    continue;
                }
                // Make the new tree with the different order and get the cost
                JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                double eval = jt->getCost();
                // Replace the old one if the new cost is better
                if (eval < best->getCost()) {
                    // std::cout << eval << " < " << best->getCost() << '\n';
                    delete best;
                    bestTree[str] = jt;
                } else {
                    delete jt;
                }
            }
        }
    }

    // Copy the best order for the predicates in query info
    str = vectorToString(relationsSet.back());
    JoinTree * jt = bestTree[str];
    delete[] queryInfo->predicates;
    copyPredicates(&queryInfo->predicates, jt->getPredicates(), queryInfo->predicatesCount);
    for (auto it: bestTree ) {
        delete it.second;
        // std::cout << it.second->getCost() << '\n';
    }
    // Predicate * predicates2 = jt->getPredicates();
    // for (size_t i = 0; i < queryInfo->predicatesCount; i++) {
    //     printPredicate(&predicates2[i]);
    // }
}
