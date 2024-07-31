#include <stdint.h>

#define PRIME_BIT_TABLE_LENGTH 1 << 26

#define PRIME_TABLE_MAX UINT32_MAX
#define PRIME_TABLE_PRIME_COUNT 203280221

uint64_t* primesU32cprs();
int64_t bigPIcprs(uint64_t* table, uint64_t blockLength);
