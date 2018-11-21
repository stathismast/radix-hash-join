#include <iostream>
#include "../singleJoin/result.hpp"

int main(void){

    Result * result = newResult();

    for(uint64_t i=0; i<100; i++){
        insertSingleResult(result, i);
    }

    printSingleResult(result);
    deleteResult(result);
}
