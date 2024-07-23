// #include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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

// int main() {
//     printf("%u\n", gcdU16(92, 128));
//     return 0;
// }
