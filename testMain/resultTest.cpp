#include <iostream>
#include "../singleJoin/result.hpp"

int main(void){

    Result * result = newResult();

    for(uint64_t i=0; i<700; i++){
        uint64_t entries[5] = {i,i,i,i,i};
        // std::cout << entries[1] << std::endl;
        insertResult(result, entries, 5);
    }

    // std::cout << result->nodesNum << " " << result->totalEntries << std::endl;

    // printResult(result,5);

    uint64_t * entry = getEntry(result, 652, 5);

    if(entry == NULL) std::cout << "Index is too high" << std::endl;
    else{
        std::cout << entry[0] << "|";
        std::cout << entry[1] << "|";
        std::cout << entry[2] << "|";
        std::cout << entry[3] << "|";
        std::cout << entry[4] << "|" << std::endl;
    }
    deleteResult(result);
}
