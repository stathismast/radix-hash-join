#include "../parse/parse.hpp"


int main(int argc, char const *argv[]) {
    QueryInfo* queryInfo = parseInput(stdin);
    delete[] queryInfo->relations;
    for (int i = 0; i < queryInfo->predicatesCount; i++) {
        delete queryInfo->predicates[i];
    }
    delete[] queryInfo->predicates;
    delete[] queryInfo->sums;
    delete queryInfo;
    return 0;
}
