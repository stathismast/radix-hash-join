#include "optimizer.hpp"
#include "predicates.hpp"
#include <vector>
#include <string>
#include <cstdlib>
#include "stats.hpp"
#include <unordered_map>
#include <unordered_set>

using std::string;

extern Relation * r;
extern uint64_t relationsSize;
extern Stats ** stats;

// constructor for the single relation nodes
JoinTree::JoinTree (uint64_t rel, QueryInfo * queryInfo) {
    this->cost = 0;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, queryInfo->predicates, this->predicatesCount);

    this->predicateStr = std::to_string(rel);
    this->set = {rel};
    // for (uint64_t i = 0; i < set.size(); i++) {
    //     std::cout << "it = " << set[i] << '\n';
    // }

    this->lastPredicate = 0;
    this->lastPredicate = reorderFilters(rel);

    this->myStats = copyStats(this->myStats, stats, queryInfo);
    // std::cout << predicateStr << ":" << '\n';
    // printStats();
    // std::cout << '\n';
    // std::cout << '\n';

    // std::cout << "For " << predicateStr << '\n';
    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&(this->predicates[i]));
    // }
    std::cout << predicateStr << ".cost = " << cost << '\n';
}


// Constructor for joins when there is no tree for the v set
JoinTree::JoinTree (std::vector<uint64_t> v, uint64_t rel, QueryInfo * queryInfo) {
    this->cost = 0;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, queryInfo->predicates, this->predicatesCount);

    this->set = makeSet(v, rel);
    // for (auto it = this->set.begin(); it != this->set.begin(); ++it) {
    //     std::cout << "it = " << *it << '\n';
    // }
    this->predicateStr = vectorToString(this->set);

    this->lastPredicate = 0;

    this->myStats = copyStats(this->myStats, stats, queryInfo);
    // std::cout << predicateStr << ":" << '\n';
    // printStats();
    // std::cout << '\n';
    // std::cout << '\n';

    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = 0; i < this->predicatesCount; i++) {
        // Find which relations are joind and bring them first, also calculate stats
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
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }
    this->cost += myStats[rel][0].f;
    this->lastPredicate = orderedCount;

    // std::cout << "For " << predicateStr << '\n';
    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&(this->predicates[i]));
    // }
    std::cout << predicateStr << ".cost = " << cost << '\n';
}


// Constructor for joins
JoinTree::JoinTree (JoinTree * jt, uint64_t rel, QueryInfo * queryInfo) {
    this->cost = jt->cost;

    this->relations = queryInfo->relations;
    this->relationsCount = queryInfo->relationsCount;
    this->predicatesCount = queryInfo->predicatesCount;
    copyPredicates(&this->predicates, jt->predicates, this->predicatesCount);

    // for (auto it = jt->set.begin(); it != jt->set.begin(); ++it) {
    //     std::cout << "it = " << *it << '\n';
    // }
    this->set = jt->set;

    this->myStats = copyStats(this->myStats, jt->myStats, queryInfo);
    // std::cout << predicateStr << ":" << '\n';
    // printStats();
    // std::cout << '\n';
    // std::cout << '\n';

    this->lastPredicate = jt->lastPredicate;
    this->lastPredicate = reorderFilters(rel);

    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     std::cout << "\t";
    //     printPredicate(&(this->predicates[i]));
    // }
    // std::cout << "jt->set = " << vectorToString(jt->set) << '\n';


    uint64_t orderedCount = this->lastPredicate;
    for (size_t i = orderedCount; i < this->predicatesCount; i++) {
        // Find which relations are joind and bring them first, also calculate stats
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = relations[predicates[i].relationA];
            uint64_t relB = relations[predicates[i].relationB];
            uint64_t colA = predicates[i].columnA;
            uint64_t colB = predicates[i].columnB;

            bool f1 = isInVector(jt->set, relA);
            bool f2 = isInVector(jt->set, relB);
            if ( (f1 || f2) && (rel == relA || rel == relB)) {
                // relA = predicates[i].relationA;
                // relB = predicates[i].relationB;
                // std::cout << relA << "." << colA << " = ";
                // std::cout << relB << "." << colB << '\n';
                // the first relation will be the one on jt->set
                if (f1 == true) {
                    // std::cout << "A:" << relA << " on set" << '\n';
                    updateJoinStats(relA, colA, relB, colB);
                } else if (f2 == true) {
                    updateJoinStats(relB, colB, relA, colA);
                    // std::cout << "A:" << relB << " on set" << '\n';
                }
                // updateJoinStats(relA, colA, relB, colB);
                swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                orderedCount++;
            }
        }
    }
    this->lastPredicate = orderedCount;
    this->cost += myStats[rel][0].f;

    this->set = makeSet(jt->set, rel);
    this->predicateStr = vectorToString(this->set);

    std::cout << predicateStr << ":" << '\n';
    std::cout << ".cost = " << cost << '\n';
    printStats();
    std::cout << '\n';
    std::cout << '\n';

    // std::cout << "For " << predicateStr << '\n';
    // for (size_t i = 0; i < this->predicatesCount; i++) {
    //     printPredicate(&(this->predicates[i]));
    // }
}

void JoinTree::updateStats(uint64_t rel, uint64_t col, Stats newStats){
    myStats[rel][col].l = newStats.l;
    myStats[rel][col].u = newStats.u;
    myStats[rel][col].f = newStats.f;
    myStats[rel][col].d = newStats.d;
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
    for (uint64_t j = 0; j < set.size(); j++) {
        // std::cout << "it = " << set[i] << '\n';
        uint64_t setRel = set[j];
        if (setRel != rel ) {
            std::cout << "Less updating " << setRel << '\n';
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
    for (uint64_t j = 0; j < set.size(); j++) {
        // std::cout << "it = " << set[i] << '\n';
        uint64_t setRel = set[j];
        if (setRel != rel ) {
            std::cout << "Greater updating " << setRel << '\n';
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
    for (uint64_t j = 0; j < set.size(); j++) {
        // std::cout << "it = " << set[j] << '\n';
        uint64_t setRel = set[j];
        std::cout << "setRel = " << setRel << '\n';
        if (setRel != relB && setRel != relA) {
            std::cout << "Updating " << setRel << '\n';
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

double JoinTree::evalCost(uint64_t rel) {
    Stats newStats;
    for (size_t i = 0; i < this->predicatesCount; i++) {
        // Find which relations are joind and bring them first, also calculate stats
        if (predicates[i].predicateType == JOIN) {
            uint64_t relA = relations[predicates[i].relationA];
            uint64_t relB = relations[predicates[i].relationB];
            uint64_t colA = relations[predicates[i].columnA];
            uint64_t colB = relations[predicates[i].columnB];

            bool f1 = isInVector(this->set, relA);
            bool f2 = isInVector(this->set, relB);
            if ( (f1 || f2) && (rel == relA || rel == relB)) {
                newStats = evalJoinStats(relA, colA, relB, colB);
                // updateJoinStats(relA, colA, relB, colB);
                // swapPredicates(&(this->predicates[i]), &(this->predicates[orderedCount]));
                // orderedCount++;
            }
        }
    }
    return newStats.f + this->cost;
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
            std::cout << curStr << " and " << rel << " are connected, so set = " << str << '\n';
            if (curTree == NULL) {
                // std::cout << "--No tree for curTree (" << curStr << ")" << '\n';
                JoinTree * jt = new JoinTree(cur, rel, queryInfo);
                bestTree[curStr] = jt;
            } else {
                JoinTree * best = bestTree[str];
                std::cout << "--Tree for curTree (" << curStr << ")" << '\n';
                if (best == NULL) {
                    // std::cout << "\tNo tree for " << str << '\n';
                    JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                    bestTree[str] = jt;
                } else {
                    std::cout << "\tTree for " << str << '\n';
                    // In case we have a tree with a set of 2 relations it means
                    // we just check for the reverse set. For example if we
                    // have 01 the, second time we check for 10. We don't need
                    // to do anything in this case
                    if (str.length() == 2) {
                        std::cout << "Lenght 2, not doing anything" << '\n';
                        continue;
                    }
                    // JoinTree * jt2 = new JoinTree(curTree, rel, queryInfo);
                    JoinTree * jt = new JoinTree(curTree, rel, queryInfo);
                    double eval = jt->getCost();
                    // eval cost and check if it's better than the cost of best
                    // double eval = curTree->evalCost(rel);
                    // double eval = 200;
                    std::cout << "eval = " << eval << '\n';
                    if (eval < best->getCost()) {
                        bestTree[str] = jt;
                    }

                }
            }
        }
    }

    // std::cout << "Hash:" << '\n';
    // for (auto x = bestTree.begin(); x!=  bestTree.end(); x++) {
    //     std::cout << x->first << ":  " << x->second->getPredicateStr() << ", " << x->second->getCost() << '\n';
    // }
    std::cout << '\n';
    std::cout << "Down here" << '\n';
    JoinTree * bestT = bestTree["014"];
    // JoinTree * bestT = bestTree["14"];
    bestT->printStats();
    Predicate * predicates2 = bestT->getPredicates();
    for (size_t i = 0; i < 3; i++) {
        printPredicate(&predicates2[i]);
    }
}
