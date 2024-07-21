/*

    This file is used to generate the first entry of `tailTable`.
    Make sure the number printed out is 64 bit. It should output: 
    
        0xa835be21f89e39ac

    You may need to adjust the printf formatting.
*/

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

    printf("0x%llx\n", liouvilleBlock(0));
    printf("0x%lx\n", liouvilleBlock(0));

    return 0;
}
