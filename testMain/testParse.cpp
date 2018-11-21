#include "../parse/parse.hpp"


int main(int argc, char const *argv[]) {
    QueryInfo* queryInfo = parseInput(stdin);
    delete[] queryInfo->relations;
    for (int i = 0; i < queryInfo->predicatesCount; i++) {
        delete queryInfo->predicates[i];
    }
    deleteQueryInfo(queryInfo);
    return 0;
}
