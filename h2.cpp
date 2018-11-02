#include "h2.hpp"

int32_t h2(int32_t value, uint32_t prime){
    // uint32_t prime = nextPrime(start);
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

void bucketify2 (relation * rel,
                    uint32_t bucket_size,
                    uint32_t start,
                    int ** bucket_array,
                    int ** chain_array) {
    // Normaly bucket_count, which is the number of buckets, would be based on
    // our hash function which won't be neccessarily a mod with a prime
    // For starters we take it that way so we can see if it works
    uint32_t i, j, bucket_count;
    int prime = nextPrime(bucket_size), hash_value, chain_pos;
    bucket_count = prime;
    *bucket_array = new int[bucket_count];
    *chain_array = new int[bucket_size];

    // Initialise our arrays
    for (j = 0; j < bucket_size; j++) {
        (*chain_array)[j] = -1;
    }
    for (j = 0; j < bucket_count; j++) {
        (*bucket_array)[j] = -1;
    }

    // Find in which bucket each element is and make the arrays for the bucket
    // and the chain
    for (i = bucket_size + start - 1; i >= start; i--) {
        hash_value = h2(rel->column[i].value, prime);
        // If there is nothing in the bucket then the bucket will only point
        // at it
        if ((*bucket_array)[hash_value] == -1) {
            (*bucket_array)[hash_value] = i - start;
        }
        else {
            // If we have a collision we need to add a value in the chain array
            // We look in order to find which was the last element added to this
            // bucket
            chain_pos = (*bucket_array)[hash_value];
            while ((*chain_array)[chain_pos] != -1) {
                chain_pos = (*chain_array)[chain_pos];
            }
            // After we find it we change its value so it will point in the
            // current element
            (*chain_array)[chain_pos] = i - start;
        }
        // because i is unsigned it will never drop below 0 and the condition
        // will never be false in the first loop
        if (i == 0) {
            break;
        }
    }

    // Debugging prints
    // std::cout << "Ordered bucket:" << std::endl;
    // for (i = start; i < bucket_size + start; i++) {
    //     std::cout << "\t"<<  i - start << ": " << rel->column[i].value << std::endl;
    // }
    // std::cout << std::endl;
    //
    // std::cout << "Bucket_array:" << std::endl;
    // for (uint32_t i = 0; i < bucket_count; i++) {
    //     std::cout << "\t" << i << ": " << (*bucket_array)[i] << std::endl;
    // }
    // std::cout << std::endl;
    //
    // std::cout << "Chain_array:" << std::endl;
    // for (uint32_t i = 0; i < bucket_size; i++) {
    //     std::cout << "\t" << i << ": " << (*chain_array)[i] << std::endl;
    // }
}
