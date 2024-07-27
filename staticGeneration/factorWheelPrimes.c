/*
    This is to be used with larger values to check for primes
    maybe. A table like this is not a very good idea anyways.

    yeah probably gonna delete later
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define HEAD_TABLE_SIZE 30030

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

int main() {

    uint32_t ct = 0;

    for(uint16_t i = 0; i < HEAD_TABLE_SIZE; i++) {
        uint16_t gcd = gcdU16(HEAD_TABLE_SIZE, i);
    
        if(gcd != 1) continue;

        printf("%u, ", i);
        ct++;
    }
    
    printf("\n%u, \n", ct);

    return 0;
}
