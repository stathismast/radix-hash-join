#include "h2.hpp"

int32_t h2(int32_t value, uint32_t start){
    uint32_t prime = nextPrime(start);
    return value % prime;
}

void test_nextPrime(uint32_t upTo){
    std::cout << "Testing nextPrime up to " << upTo << std::endl;
    for(uint32_t i=0; i<upTo; i++){
        std::cout << i << ". " << nextPrime(i) << std::endl;
    }
}

uint32_t nextPrime(uint32_t start){
    // Make sure we start checking from an odd number larger than 'start'
    if(start%2 == 0) start+=1;
    else start+=2;

    // Variable 'i' will be our next potential prime
    uint32_t i = start;

    while(1){
        if(isPrime(i)) break;
        i+=2;   // Increase i by two since there are no even primes
    }

    // The above while loop can be replaced by:
    // for(i = start; !isPrime(i); i+=2){}

    return i;
}

char isPrime(uint32_t i){
    uint32_t d = 3; // First possible divisor
    char flag = 1;  // Flag to stop the search if we find a divisor

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
