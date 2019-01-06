#include "stats.hpp"

extern Relation * r;
extern uint64_t relationsSize;
extern Stats ** stats;

void createStats(){
    stats = new Stats*[relationsSize];
    for(uint64_t i=0; i<relationsSize; i++){
        stats[i] = new Stats[r[i].cols];
    }
}

void deleteStats(){
    for(uint64_t i=0; i<relationsSize; i++){
        delete[] stats[i];
    }
    delete[] stats;
}

// Create a new Stats array with the statistics in 'r'
void initializeStats(){
    for(uint64_t i=0; i<relationsSize; i++){
        for(uint64_t j=0; j<r[i].cols; j++){
            stats[i][j].l = r[i].l[j];
            stats[i][j].u = r[i].u[j];
            stats[i][j].f = r[i].f[j];
            stats[i][j].d = r[i].d[j];
        }
    }
}

void equalFilter(uint64_t rel, uint64_t col, uint64_t k){
    double l = stats[rel][col].l;
    double u = stats[rel][col].u;
    double f = stats[rel][col].f;
    double d = stats[rel][col].d;

    Stats newStats;
    newStats.l = k;
    newStats.u = k;

    if(k < l || k > u){
        // If k is not in the range [l,u]
        newStats.f = 0;
        newStats.d = 0;
    }
    else if(d == 0){
        // Check if d is 0 to avoid dividing by 0 in the next block
        newStats.f = 0;
        newStats.d = 0;
    }
    else if(f == 0){
        // Check if f is 0 meaning there are not entries
        newStats.f = 0;
        newStats.d = 0;
    }
    else{
        // If k is in the range [l,u] and there is atleast one distinct value
        newStats.f = f/d;
        newStats.d = 1;
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        uint64_t dc = stats[rel][i].d;
        uint64_t fc = stats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        }
        else if(f == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        } else {
            std::cout << fc/dc << std::endl;
            std::cout << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}


void lessFilter(uint64_t rel, uint64_t col, uint64_t k) {
    double l = stats[rel][col].l;
    double u = stats[rel][col].u;
    double f = stats[rel][col].f;
    double d = stats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    newStats.l = stats[rel][col].l;
    if (k > u) {
        k = u;
    }
    newStats.u = k;
    if (k < l) {
        // if filter value is lower than the lowest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (stats[rel][col].l == stats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = d*(k-l)/(u-l);
        newStats.f = f*(k-l)/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        uint64_t dc = stats[rel][i].d;
        uint64_t fc = stats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        }
        else if(f == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        } else {
            std::cout << fc/dc << std::endl;
            std::cout << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}

void greaterFilter(uint64_t rel, uint64_t col, uint64_t k) {
    double l = stats[rel][col].l;
    double u = stats[rel][col].u;
    double f = stats[rel][col].f;
    double d = stats[rel][col].d;

    Stats newStats;
    // The lowest value after the filter execution will still be the same and
    // the highest will be k
    if (k < l) {
        k = l;
    }
    newStats.l = k;
    newStats.u = stats[rel][col].u;
    if (k > u) {
        // if filter value is higher than the highest value there will be no results
        newStats.d = 0;
        newStats.f = 0;
    }
    else if (stats[rel][col].l == stats[rel][col].u) {
        // check if u and l of current collumn are not equal to avoid division
        // by zero
        newStats.d = 0;
        newStats.f = 0;
    }
    else {
        newStats.d = d*(u-k)/(u-l);
        newStats.f = f*(u-k)/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        uint64_t dc = stats[rel][i].d;
        uint64_t fc = stats[rel][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        }
        else if(f == 0){
            stats[rel][i].d = 0;
            stats[rel][i].f = 0;
        } else {
            std::cout << fc/dc << std::endl;
            std::cout << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}

void updateStats(uint64_t rel, uint64_t col, Stats newStats){
    stats[rel][col].l = newStats.l;
    stats[rel][col].u = newStats.u;
    stats[rel][col].f = newStats.f;
    stats[rel][col].d = newStats.d;
}
