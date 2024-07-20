#include <stdio.h>
#include <stdint.h>
#include <math.h>

// 8! because it is the size of modern CPU caches and the value is almost 16 bits
#define TABLE_FACTORIAL_BASE 8
#define TABLE_SIZE 40320

void generateLookupTable(uint16_t* divisorTable, uint8_t* oddFactorCount) {
    divisorTable[0] = 1;
    oddFactorCount[0] = 0;

    for(uint16_t i = 1; i < TABLE_SIZE; i++) {
        uint16_t workingI = i;
        
        uint16_t divisor = 1;
        uint8_t factors = 0;

        for(uint8_t d = 2; d < TABLE_FACTORIAL_BASE; d++) {
            if(d == 4 || d == 6) continue;
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

uint8_t liouville_full(uint64_t n) {
    if (n == 0) return 0;
    uint8_t factorCount = 0;

    uint16_t trailingZeroCount = __builtin_ctz(n);
    factorCount ^= trailingZeroCount & 1;
    n = n >> trailingZeroCount;

    while(n % 3 == 0) {
        factorCount ^= 1;
        n /= 3;
    }
    
    for(uint32_t div = 6; div <= (uint32_t)sqrt(n) + 1; div += 6) {
        while(n % (div - 1) == 0) {
            factorCount ^= 1;
            n /= div - 1;
        }
        while(n % (div + 1) == 0) {
            factorCount ^= 1;
            n /= div + 1;
        }
    }
 
    // case where the number is prime after the 2 and 3 test
    factorCount  ^= n > 2;

    return factorCount;
}

uint8_t applyLookUpTable(uint64_t n, uint16_t lookUpTablePosition, uint16_t* divisorTable, uint8_t* oddFactorCount) {
    return liouville_full(n / (uint64_t)divisorTable[lookUpTablePosition]) ^ oddFactorCount[lookUpTablePosition];
}

uint64_t liouvilleBlock(uint64_t starting) {
    uint64_t block = 0;
    for(uint64_t i = 0; i < 64; i++) {
        block ^= (uint64_t)liouville_full(starting + i) << i;
    }
    
    return block;
}

int main() {

    uint16_t firstDivisorTable[TABLE_SIZE];
    uint8_t tableOddFactor[TABLE_SIZE];

    generateLookupTable(firstDivisorTable, tableOddFactor);

    int32_t sum = -1; // start at -1 to account for liouville_full(0)
    // for(uint16_t i = 0; i < 1563; i++) {
        // sum += __builtin_popcountll(liouvilleBlock(i * 64)) * -2 + 64;
        // printf("%u %ld\n", i * 64 + 63, sum);
    // }

    for(uint32_t i = 1; i < 100000000; i++) {
        // sum += liouville_full(i);
        // sum += applyLookUpTable(i, i % TABLE_SIZE, firstDivisorTable, tableOddFactor);
    }
    printf("%u\n", sum);

    return 0;
}
