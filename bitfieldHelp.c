#include "bitfieldHelp.h"

void setBitFromZero(uint64_t* field, uint64_t offset, uint8_t value) {
    field[offset / 64] |= (uint64_t)value << (offset % 64) & 1;
}

uint8_t getBit(uint64_t* field, uint64_t offset) {
    return (uint8_t)(field[offset / 64] >> (offset % 64)) & 1;
}
