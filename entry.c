#include <stdio.h>
#include <stdint.h>
#include <math.h>

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

uint64_t liouvilleBlock(uint64_t starting) {
    uint64_t block = 0;
    for(uint64_t i = 0; i < 64; i++) {
        block ^= (uint64_t)liouville_full(starting + i) << i;
    }
    
    return block;
}

int main() {

    int32_t sum = -1; // start at -1 to account for liouville_full(0)
    for(uint16_t i = 0; i < 10; i++) {
        sum += __builtin_popcountll(liouvilleBlock(i * 64)) * -2 + 64;
        printf("%u %ld\n", i * 64 + 63, sum);
    }

    // for(uint32_t i = 1; i < 100000; i++) {
    //     liouville_fullADD(i);
    // }

    return 0;
}
