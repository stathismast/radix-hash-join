#include "../list.hpp"

int main(int argc, char const *argv[]) {
    Result my_results;
    my_results = newResult();

    insertResult(my_results,1,2);
    insertResult(my_results,2,1);
    insertResult(my_results,1,3);
    insertResult(my_results,3,1);

    printResult(my_results);

    destroyResult(my_results);
    return 0;
}
