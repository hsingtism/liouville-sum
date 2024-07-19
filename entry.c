#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define TESTBLOCK 128

uint32_t liouville_full(uint32_t n) {
    if (n == 0) return 0;
    uint8_t factorCount = 0;

    uint16_t trailingZeroCount = __builtin_ctz(n);
    factorCount += trailingZeroCount;
    n = n >> trailingZeroCount;

    while(n % 3 == 0) {
        factorCount++;
        n /= 3;
    }
    
    for(uint32_t div = 6; div <= (uint32_t)sqrt(n) + 1; div += 6) {
        while(n % (div - 1) == 0) {
            factorCount++;
            n /= div - 1;
        }
        while(n % (div + 1) == 0) {
            factorCount++;
            n /= div + 1;
        }
    }
 
    // case where the number is prime after the 2 and 3 test
    factorCount += n > 2;

    return factorCount;
}

int main() {

    for(uint32_t i = 1; i < TESTBLOCK; i++) {
        printf("%u %u\n", i, liouville_full(i));
    }

    return 0;
}
