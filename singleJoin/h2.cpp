#include "h2.hpp"

uint64_t h2(uint64_t value, uint64_t prime){
    // uint64_t prime = nextPrime(start);
    return value % prime;
}

void testNextPrime(uint64_t upTo){
    std::cout << "Testing nextPrime up to " << upTo << std::endl;
    for(uint64_t i=0; i<upTo; i++){
        std::cout << i << ". " << nextPrime(i) << std::endl;
    }
}

uint64_t nextPrime(uint64_t start){
    // Make sure we start checking from an odd number larger than 'start'
    if(start%2 == 0) start+=1;
    else start+=2;

    // Variable 'i' will be our next potential prime
    uint64_t i = start;

    while(1){
        if(isPrime(i)) break;
        i+=2;   // Increase i by two since there are no even primes
    }

    // The above while loop can be replaced by:
    // for(i = start; !isPrime(i); i+=2){}

    return i;
}

bool isPrime(uint64_t i){
    uint64_t d = 3; // First possible divisor
    bool flag = 1;  // Flag to stop the search if we find a divisor

    // While the divisor is less than the square root of out the number
    // that we're checking & we haven't found a divisor yet
    while(d*d <= i && flag){
        // Check if 'd' is a divisor of 'i'
        if(i%d == 0)
            flag = 0;

        // Increase the divisor by two, no need to check for even divisors
        d+=2;
    }

    // If we found no divisors, 'flag' is 'true' and 'i' is prime
    // Otherwise, 'flag' will be 'false'
    return flag;
}

void bucketify2(Column * rel,
                uint64_t bucketSize,
                uint64_t startingPos,
                uint64_t ** bucketArray,
                uint64_t ** chainArray) {

    uint64_t prime = nextPrime(bucketSize);

    *bucketArray = new uint64_t[prime];
    *chainArray = new uint64_t[bucketSize];

    // Initialise our arrays
    for (uint64_t j = 0; j < bucketSize; j++) {
        (*chainArray)[j] = -1;
    }
    for (uint64_t j = 0; j < prime; j++) {
        (*bucketArray)[j] = -1;
    }

    // Find in which bucket each element is and make the arrays for the bucket
    // and the chain
    for (uint64_t i = startingPos + bucketSize - 1; i >= startingPos; i--) {

        // Get the hashed value
        uint64_t hashValue = h2(rel->value[i], prime);

        // If the corresponding bucket is currently empty
        if ((*bucketArray)[hashValue] == (uint64_t) -1) {
            (*bucketArray)[hashValue] = i - startingPos;
        }
        else {
            // If the bucket is not empty, we have a collision,
            // so we need to add a value in the chain array
            uint64_t chainPos = (*bucketArray)[hashValue];
            while ((*chainArray)[chainPos] != (uint64_t) -1) {
                chainPos = (*chainArray)[chainPos];
            }
            (*chainArray)[chainPos] = i - startingPos;
        }

        // Since 'i' is unsigned it will never drop below 0 and
        // the condition will never be false in the first loop
        if (i == 0) {
            break;
        }
    }

    // Debugging prints
    // std::cout << "Ordered bucket:" << std::endl;
    // for (uint64_t i = startingPos; i < bucketSize + startingPos; i++) {
    //     std::cout << "\t"<<  i - startingPos  << ": "
    //     << rel->value[i] << ": "
    //     << h2(rel->value[i], prime) << ": "
    //     << prime << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "bucketArray:" << std::endl;
    // for (uint64_t i = 0; i < prime; i++) {
    //     std::cout << "\t" << i << ": " << (*bucketArray)[i] << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "chainArray:" << std::endl;
    // for (uint64_t i = 0; i < bucketSize; i++) {
    //     std::cout << "\t" << i << ": " << (*chainArray)[i] << std::endl;
    // }
}
