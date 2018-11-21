#include <iostream>
#include "../singleJoin/result.hpp"

int main(void){

    Result * result = newResult();

    for(uint64_t i=0; i<350; i++){
        uint64_t entries[5] = {i,i,i,i,i};
        // std::cout << entries[1] << std::endl;
        insertResult(result, entries, 5);
    }

    // std::cout << result->nodesNum << " " << result->totalEntries << std::endl;

    printResult(result,5);
    deleteResult(result);
}
