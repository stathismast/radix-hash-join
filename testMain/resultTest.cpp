#include <iostream>
#include "../join/intermediate.hpp"

int main(void){

    Result * result = newResult();

    for(uint64_t i=0; i<70000; i++){
        uint64_t entries[5] = {i,2*i,3*i,4*i,5*i};
        // std::cout << entries[1] << std::endl;
        insertResult(result, entries, 5);
    }

    // std::cout << result->nodesNum << " " << result->totalEntries << std::endl;

    // printResult(result,5);

    uint64_t * entry = getEntry(result, 65200, 5);

    if(entry == NULL) std::cout << "Index is too high" << std::endl;
    else{
        std::cout << entry[0] << "|";
        std::cout << entry[1] << "|";
        std::cout << entry[2] << "|";
        std::cout << entry[3] << "|";
        std::cout << entry[4] << "|" << std::endl;
    }

    Column * column = resultToColumn(result, 4, 5);
    // printColumn(column);

    std::cout << column->value[65200] << std::endl;

    if(entry[4] == column->value[65200])
        std::cout << "All working well." << std::endl;

    deleteColumn(column);

    deleteResult(result);
}
