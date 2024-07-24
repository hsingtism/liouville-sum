#include <stdint.h>
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


// lots of help from https://github.com/going-digital/Prime64/blob/master/Prime64.cpp
uint64_t modularMultiplication(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t t = 0;

    while(b) {
        if(b & 1) {
            uint64_t t2 = t + a;
            if(t2 < t) t2 -= mod;
            t = t2 % mod; 
        }

        b >>= 1;

        if(b) {
            uint64_t a2 = a << 1;
            if(a2 < a) a2 -= mod;
            a = a2 % mod;
        }
    }

    return t;
}

uint64_t modularExponentiation(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t t = 1;

    while(exp) {
        if(exp & 1) {
            t = modularMultiplication(t, base, mod);
        }

        exp >>= 1;

        if(exp) {
            base = modularMultiplication(base, base, mod);
        }
    }

    return t;
}

// deterministic for all u64
// this is relatively slow, use as a last resort when all wheel methods are unavilable
uint8_t millerRabinU64(uint64_t n) {
    if(n & 1 == 0) return 0;

    uint64_t d = n - 1;
    uint8_t s = __builtin_ctzll(d);

    uint8_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

    for(uint8_t tn = 0; tn < 12; tn++) {
        uint8_t a = bases[tn];
        uint64_t y;

        uint64_t x = modularExponentiation(a, d, n);
        for(uint8_t i = 0; i < s; i++) {
            y = modularMultiplication(x, x, n);
            if(y == 1 && x != 1 && x != n - 1) {
                return 0;
            }
        }
        if(y != 1) {
            return 0;
        }
    }

    return 1;
}
