#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

uint16_t gcdU16(uint16_t a, uint16_t b);
uint8_t liouville_full(uint64_t n);
