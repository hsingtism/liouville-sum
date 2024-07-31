#include "maths.h"

#include <stdlib.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint16_t gcdU16(uint16_t a, uint16_t b) {
    if(a * b == 0) {
        return a | b; 
    }

    uint16_t commonTZ = __builtin_ctz(a | b);
    a >>= commonTZ;
    b >>= commonTZ;

    a >>= __builtin_ctz(a);

    do {
        b >>= __builtin_ctz(b);

        uint16_t temp = b;
        b = MAX(a, b);
        a = MIN(a, temp);

        b -= a;
    } while (b);

    return a << commonTZ;
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
