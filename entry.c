#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// 8! because it is the size of modern CPU caches and the value is almost 16 bits
// ! do not edit
#define TABLE_FACTORIAL_BASE 8
#define HEAD_TABLE_SIZE 40320

// purely arbitrary
#define TAIL_TABLE_SIZE 10000000
#define TAIL_TABLE_FIRST_ENTRY 0xa835be21f89e39ac // see tailTableForstEntry.c

void generateHeadTable(uint16_t* divisorTable, uint8_t* oddFactorCount) {
    divisorTable[0] = HEAD_TABLE_SIZE;
    oddFactorCount[0] = 1; // liouville_full(HEAD_TABLE_SIZE) is 1

    for(uint16_t i = 1; i < HEAD_TABLE_SIZE; i++) {
        uint16_t workingI = i;
        
        uint16_t divisor = 1;
        uint8_t factors = 0;

        for(uint8_t d = 2; d < TABLE_FACTORIAL_BASE; d++) {
            if(d == 4 || d == 6) continue; // primes only
            if(workingI % d == 0) {
                divisor *= d;
                factors ^= 1;
            }
        }

        oddFactorCount[i] = factors;
        divisorTable[i] = divisor;
        // printf("%u %u %u ;;", i, factors, divisor);
    }
}

uint8_t liouvilleLookup(uint64_t n, uint64_t* tailTable, uint16_t* divisorTable, uint8_t* oddFactorCount) {
    
    const uint64_t initial = n;
    uint8_t factorCount = 0;

    // ! do not switch these two statements
    factorCount ^= oddFactorCount[n % HEAD_TABLE_SIZE];
    n /= divisorTable[n % HEAD_TABLE_SIZE];

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)(tailTable[n / 64] >> (n % 64)) & 1;
    }

    // return liouville_full(n) ^ factorCount;

    uint16_t trailingZeroCount = __builtin_ctz(n);
    factorCount ^= trailingZeroCount & 1;
    n = n >> trailingZeroCount;

    // TODO don't repeat this
    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
    }

    while(n % 3 == 0) {
        factorCount ^= 1;
        n /= 3;
    }
    
    // ! here
    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
    }

    for(uint32_t div = 6; div <= (uint32_t)sqrt(n) + 1; div += 6) {
        while(n % (div - 1) == 0) {
            factorCount ^= 1;
            n /= div - 1;
        }

        // ! here
        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
        }

        while(n % (div + 1) == 0) {
            factorCount ^= 1;
            n /= div + 1;
        }

        // ! here
        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
        }

    }
 
    // case where the number is prime after the 2 and 3 test
    factorCount  ^= n > 2;

    return factorCount;
}

uint64_t liouvilleBlockLookup(uint64_t starting, uint64_t* tailTable, uint16_t* divisorTable, uint8_t* oddFactorCount) {
    uint64_t block = 0;
    for(uint64_t i = 0; i < 64; i++) {
        block ^= (uint64_t)liouvilleLookup(starting + i, tailTable, divisorTable, oddFactorCount) << i;
    }
    
    return block;
}

int main() {

    int32_t sum = -1; // start at -1 to account for liouville_full(0)
    
    uint16_t headTableDivisor[HEAD_TABLE_SIZE];
    uint8_t headTableFactor[HEAD_TABLE_SIZE];
    generateHeadTable(headTableDivisor, headTableFactor);

    // TODO use malloc
    // uint64_t tailTable[TAIL_TABLE_SIZE];

    uint64_t* tailTable = malloc(TAIL_TABLE_SIZE * sizeof(uint64_t));

    tailTable[0] = TAIL_TABLE_FIRST_ENTRY;
    sum += __builtin_popcountll(tailTable[0]) * -2 + 64;
    

    for(uint32_t i = 1; i < 1562500; i++) {
    // for(uint32_t i = 1; i < 15; i++) {
        uint64_t block = liouvilleBlockLookup(i * 64, tailTable, headTableDivisor, headTableFactor);
        sum += __builtin_popcountll(block) * -2 + 64;

        if(i < TAIL_TABLE_SIZE) {
            tailTable[i] = block;
        }

        if(i % 8192 == 0) {
            printf("%u %d\n", i * 64 + 63, sum);
        }

    }


    // for(uint16_t i = 0; i < 10; i++) {
    // for(uint16_t i = 64; i < 84; i++) {

        // printf("%u %ld\n", i, liouvilleLookup(i, tailTable, headTableDivisor, headTableFactor));
        

        // sum += __builtin_popcountll(liouvilleBlock(i * 64)) * -2 + 64;
        // printf("%u %ld\n", i * 64 + 63, sum);
    // }

    // for(uint32_t i = 1; i < 100000000; i++) {
        // sum += liouville_full(i);
        // sum += applyLookUpTable(i, i % HEAD_TABLE_SIZE, headTableDivisor, headTableFactor);
    // }
    printf("%d\n", sum);

    return 0;
}
