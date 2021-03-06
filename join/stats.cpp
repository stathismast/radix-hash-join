#include "stats.hpp"

extern Relation * r;
extern uint64_t relationsSize;
extern Stats ** stats;


double max(double a, double b) {
    return a > b ? a : b;
}

double min(double a, double b) {
    return a < b ? a : b;
}

Stats ** createStats(){
    Stats** stats = new Stats*[relationsSize];
    for(uint64_t i=0; i<relationsSize; i++){
        stats[i] = new Stats[r[i].cols];
    }
    return stats;
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

void updateEqualFilterStats(uint64_t rel, uint64_t col, uint64_t k){
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

        double dc = stats[rel][i].d;
        double fc = stats[rel][i].f;
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
            // std::cerr << fc/dc << std::endl;
            // std::cerr << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}


void updateLessFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
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
        newStats.d = (d*(k-l))/(u-l);
        newStats.f = (f*(k-l))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = stats[rel][i].d;
        double fc = stats[rel][i].f;
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
            // std::cerr << fc/dc << std::endl;
            // std::cerr << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}

void updateGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
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
        newStats.d = (d*(u-k))/(u-l);
        newStats.f = (f*(u-k))/(u-l);
    }

    updateStats(rel,col,newStats);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == col) continue;

        double dc = stats[rel][i].d;
        double fc = stats[rel][i].f;
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
            // std::cerr << fc/dc << std::endl;
            // std::cerr << 1-(newStats.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStats.f/f)), fc/dc));
            stats[rel][i].f = newStats.f;
        }
    }
}

void updateSelfJoinStats(uint64_t rel, uint64_t colA, uint64_t colB) {

    if (colA == colB) {
        /* code */
        std::cerr << "Corelation" << '\n';
        // call proper function
        return;
    }
    double f = stats[rel][colA].f;
    double d = stats[rel][colA].d;

    Stats newStatsA;
    Stats newStatsB;
    if (stats[rel][colA].u < stats[rel][colB].l || stats[rel][colB].u < stats[rel][colA].l) {
        // if the 2 columns have no common elemnts the result will be empty
        newStatsA.l = newStatsA.u = newStatsA.d = newStatsA.f = 0;
        newStatsB.l = newStatsB.u = newStatsB.d = newStatsB.f = 0;
    }
    else {
        newStatsA.l = newStatsB.l = max(stats[rel][colA].l, stats[rel][colB].l);
        newStatsA.u = newStatsB.u = min(stats[rel][colA].u, stats[rel][colB].u);
        double n = newStatsA.u - newStatsA.l + 1;
        if (n == 0) {
            // avoid divison by zero (not sure if it's correc to set f,d to 0)
            newStatsA.f = newStatsB.f = 0;
        }
        else {
            newStatsA.f = newStatsB.f = f/n;
        }
        newStatsA.d = newStatsB.d = d * (1-pow((1-(newStatsA.f/f)), f/d));
    }

    updateStats(rel,colA,newStatsA);
    updateStats(rel,colB,newStatsB);
    // Update the stats of every other column of given relation
    for(uint64_t i=0; i<r[rel].cols; i++){
        if(i == colA || i == colB) continue;

        double dc = stats[rel][i].d;
        double fc = stats[rel][i].f;
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
            // std::cerr << fc/dc << std::endl;
            // std::cerr << 1-(newStatsA.f/f) << std::endl;
            stats[rel][i].d = dc * (1-pow((1-(newStatsA.f/f)), fc/dc));
            stats[rel][i].f = newStatsA.f;
        }
    }

}

void updateJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB) {
    Stats newStatsA;
    Stats newStatsB;
    double da = stats[relA][colA].d;
    double db = stats[relB][colB].d;
    // std::cerr << "relA = " << relA << ", relB = " << relB << '\n';
    // std::cerr << "colA = " << colA << ", colB = " << colB << '\n';

    double newL = max(stats[relA][colA].l, stats[relB][colB].l);
    double newU = min(stats[relA][colA].u, stats[relB][colB].u);
    // Use filter in each column so they will have same lower and upper value
    // and all other stats will be upadated accordingly
    updateGreaterFilterStats(relA, colA, newL);
    updateLessFilterStats(relA, colA, newU);
    updateGreaterFilterStats(relB, colB, newL);
    updateLessFilterStats(relB, colB, newU);

    newStatsA.l = newStatsB.l = newL;
    newStatsA.u = newStatsB.u = newU;
    double n = newU - newL + 1;
    // std::cerr << "n = " << n << '\n';
    if (n == 0) {
        newStatsA.f = newStatsA.d = newStatsB.f = newStatsB.d = 0;
    }
    else {
        newStatsA.f = newStatsB.f = (stats[relA][colA].f*stats[relB][colB].f)/n;
        newStatsA.d = newStatsB.d = (stats[relA][colA].d*stats[relB][colB].d)/n;
    }

    updateStats(relA,colA,newStatsA);
    updateStats(relB,colB,newStatsB);
    // Update the stats of every other column of the first relation
    for(uint64_t i=0; i<r[relA].cols; i++){
        if(i == colA) continue;

        double dc = stats[relA][i].d;
        double fc = stats[relA][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            stats[relA][i].d = 0;
            stats[relA][i].f = 0;
        } else {
            // std::cerr << fc/dc << std::endl;
            stats[relA][i].d = dc * (1-pow((1-(newStatsA.d/da)), fc/dc));
            stats[relA][i].f = newStatsA.f;
        }
    }

    // Update the stats of every other column of the second relation
    for(uint64_t i=0; i<r[relB].cols; i++){
        if(i == colB) continue;

        double dc = stats[relB][i].d;
        double fc = stats[relB][i].f;
        // Check if dc or f are equal to 0 and act accoridingly
        // This way we can avoid dividing by 0
        if(dc == 0){
            stats[relB][i].d = 0;
            stats[relB][i].f = 0;
        } else {
            // std::cerr << fc/dc << std::endl;
            stats[relB][i].d = dc * (1-pow((1-(newStatsB.d/db)), fc/dc));
            stats[relB][i].f = newStatsB.f;
        }
    }
}


Stats evalEqualFilterStats(uint64_t rel, uint64_t col, uint64_t k){
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

    return newStats;
}


Stats evalLessFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
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
        newStats.d = (d*(k-l))/(u-l);
        newStats.f = (f*(k-l))/(u-l);
    }

    return newStats;
}

Stats evalGreaterFilterStats(uint64_t rel, uint64_t col, uint64_t k) {
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
        newStats.d = (d*(u-k))/(u-l);
        newStats.f = (f*(u-k))/(u-l);
    }

    return newStats;
}

Stats evalSelfJoinStats(uint64_t rel, uint64_t colA, uint64_t colB) {
    Stats newStatsA;
    Stats newStatsB;

    if (colA == colB) {
        /* code */
        std::cerr << "Corelation" << '\n';
        // call proper function
        return newStatsA;
    }
    double f = stats[rel][colA].f;
    double d = stats[rel][colA].d;


    if (stats[rel][colA].u < stats[rel][colB].l || stats[rel][colB].u < stats[rel][colA].l) {
        // if the 2 columns have no common elemnts the result will be empty
        newStatsA.l = newStatsA.u = newStatsA.d = newStatsA.f = 0;
        newStatsB.l = newStatsB.u = newStatsB.d = newStatsB.f = 0;
    }
    else {
        newStatsA.l = newStatsB.l = max(stats[rel][colA].l, stats[rel][colB].l);
        newStatsA.u = newStatsB.u = min(stats[rel][colA].u, stats[rel][colB].u);
        double n = newStatsA.u - newStatsA.l + 1;
        if (n == 0) {
            // avoid divison by zero (not sure if it's correc to set f,d to 0)
            newStatsA.f = newStatsB.f = 0;
        }
        else {
            newStatsA.f = newStatsB.f = f/n;
        }
        newStatsA.d = newStatsB.d = d * (1-pow((1-(newStatsA.f/f)), f/d));
    }

    return newStatsA;
}

Stats evalJoinStats(uint64_t relA, uint64_t colA, uint64_t relB, uint64_t colB) {
    Stats newStatsA;
    Stats newStatsB;

    double newL = max(stats[relA][colA].l, stats[relB][colB].l);
    double newU = min(stats[relA][colA].u, stats[relB][colB].u);
    // Use filter in each column so they will have same lower and upper value
    // and all other stats will be upadated accordingly
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
    // std::cerr << "n = " << n << '\n';
    if (n == 0) {
        newStatsA.f = 0;
    }
    else {
        newStatsA.f = (newStatsA.f*newStatsB.f)/n;
    }

    return newStatsA;
}


void updateStats(uint64_t rel, uint64_t col, Stats newStats){
    stats[rel][col].l = newStats.l;
    stats[rel][col].u = newStats.u;
    stats[rel][col].f = newStats.f;
    stats[rel][col].d = newStats.d;
}

void printStats(uint64_t rel) {
    for (size_t i = 0; i < r[rel].cols; i++) {
        std::cerr << rel << "." << i
                    << ": l=" << stats[rel][i].l
                    << "  u=" << stats[rel][i].u
                    << "  f=" << stats[rel][i].f
                    << "  d=" << stats[rel][i].d << "\n";
    }
}

Stats ** copyStats(Stats ** target, Stats ** source, QueryInfo * queryInfo) {
    target = createStats();
    for(uint64_t i=0; i<relationsSize; i++){
        for(uint64_t j=0; j<r[i].cols; j++){
            (target[i][j]).l = source[i][j].l;
            (target[i][j]).u = source[i][j].u;
            (target[i][j]).f = source[i][j].f;
            (target[i][j]).d = source[i][j].d;
        }
    }
    return target;
}
