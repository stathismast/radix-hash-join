#include "../parse/parse.hpp"

int main(int argc, char const *argv[]) {
    QueryInfo* queryInfo = parseInput(stdin);
    deleteQueryInfo(queryInfo);
    return 0;
}
