// based on https://github.com/trizen/sidef-scripts/blob/master/Math/liouville_sum_function.sf

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

int64_t* sumTable;

#define _N 100000000
#define _N_sq (uint64_t)sqrt(_N)

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

int64_t extEval(uint64_t n) {
    if(n < _N_sq) return sumTable[n];

    const uint32_t sq = sqrt(n);
    int64_t Ln = sq;

    for(uint32_t i = 2; i <= n / (sq + 1); i++) {
        Ln -= extEval(n / i);
    }

    for(uint32_t i = 1; i <= sq; i++) {
        Ln -= sumTable[i] * (n / i - n / (i + 1));
    }

    return Ln;
}

int main() {
    sumTable = malloc((_N_sq + 1) * sizeof(int64_t));

    for(uint32_t i = 1; i <= _N_sq; i++) {
        sumTable[i] = sumTable[i - 1] + liouville_full(i) * -2 + 1;
    }

    printf("%ld\n", extEval(_N));
}
