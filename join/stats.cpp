#include "stats.hpp"

extern Relation * r;

void equalFilter(uint64_t rel, uint64_t col, uint64_t k){
    double l = r[rel].l[col];
    double u = r[rel].u[col];
    double f = r[rel].f[col];
    double d = r[rel].d[col];

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

        uint64_t dc = r[rel].d[i];
        uint64_t fc = r[rel].f[i];
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            r[rel].d[i] = 0;
            r[rel].f[i] = 0;
        }
        else if(f == 0){
            r[rel].d[i] = 0;
            r[rel].f[i] = 0;
        }
        std::cout << fc/dc << std::endl;
        std::cout << 1-(newStats.f/f) << std::endl;
        r[rel].d[i] = dc * (1-pow((1-(newStats.f/f)), fc/dc));
        r[rel].f[i] = newStats.f;
    }
}

void updateStats(uint64_t rel, uint64_t col, Stats newStats){
    r[rel].l[col] = newStats.l;
    r[rel].u[col] = newStats.u;
    r[rel].f[col] = newStats.f;
    r[rel].d[col] = newStats.d;
}
