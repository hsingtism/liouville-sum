#include "output.h"
#include "bitfieldHelp.h"
#include "primes.h"
#include "maths.h"

#include <pthread.h>
#include <stdlib.h>

uint64_t* primeBitsU32;

// https://oeis.org/A002110
#define HEAD_TABLE_SIZE 30030

#define TAIL_TABLE_SIZE (uint64_t)1000000000
#define TAIL_TABLE_FIRST_ENTRY 0xa835be21f89e39ac // see tailTableForstEntry.c

#define CPU_COUNT 32

uint16_t* headTableDivisor;
uint8_t* headTableFactor;
uint64_t* tailTable;

int is_prime_2_64(uint64_t a);

uint8_t liouvilleLookup(uint64_t n) {
    
    const uint64_t initial = n;
    uint8_t factorCount = 0;

    const uint16_t trailingZeroCount = __builtin_ctz(n);
    factorCount ^= trailingZeroCount & 1;
    n = n >> trailingZeroCount;

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ getBit(tailTable, n);
    }

    factorCount ^= headTableFactor[n % HEAD_TABLE_SIZE];
    n /= headTableDivisor[n % HEAD_TABLE_SIZE];

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ getBit(tailTable, n);
    }

    // this is still needed (maybe) in case one application of the
    // head table still left factors. This only affects value after
    // the tail table has ended (64,000,000,000)
    while(n % 3 == 0) { factorCount ^= 1; n /= 3; }
    while(n % 5 == 0) { factorCount ^= 1; n /= 5; }
    while(n % 7 == 0) { factorCount ^= 1; n /= 7; }
    while(n % 11 == 0) { factorCount ^= 1; n /= 11; }
    while(n % 13 == 0) { factorCount ^= 1; n /= 13; }

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ getBit(tailTable, n);
    }

    if(n < PRIME_TABLE_MAX && getBit(primeBitsU32, n)) {
        return factorCount ^ 1;
    } else if (headTableDivisor[n % HEAD_TABLE_SIZE] == 1 && is_prime_2_64(n)) {
        return factorCount ^ 1;
    }
    
    for(uint32_t div = 18; div <= n / (div - 1) + 1; div += 6) {

        // TODO don't use gotos
        if(getBit(primeBitsU32, div - 1) == 0) {
            goto nextTest;
        }

        while(n % (div - 1) == 0) {
            factorCount ^= 1;
            n /= div - 1;
        }

        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ getBit(tailTable, n);
        }

        nextTest:

        if(getBit(primeBitsU32, div + 1) == 0) continue;

        while(n % (div + 1) == 0) {
            factorCount ^= 1;
            n /= div + 1;
        }

        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ getBit(tailTable, n);
        }

    }
 
    factorCount  ^= n > 2;
    return factorCount;
}

uint64_t liouvilleBlockLookup(uint64_t starting) {
    if(starting == 0) {
        return TAIL_TABLE_FIRST_ENTRY;
    }

    uint64_t block = 0;
    for(uint64_t i = 0; i < 64; i++) {
        block ^= (uint64_t)liouvilleLookup(starting + i) << i;
    }
    
    return block;
}

struct ThreadData {
    uint64_t* data;
    uint32_t arrayOffset;
    uint64_t startingBlock;
    uint32_t blockCount;
};

void* threadRoutine(void* arguments) {
    struct ThreadData args = *(struct ThreadData*) arguments;
    
    for(uint64_t i = 0; i < args.blockCount; i++) {
        const uint64_t absolutePosition = args.startingBlock + i;
        
        uint64_t block = liouvilleBlockLookup(absolutePosition * 64);
        args.data[i + args.arrayOffset] = block;
        if(absolutePosition < TAIL_TABLE_SIZE) {
            tailTable[absolutePosition] = block;
        }
    }

    return NULL;
}


// blockSize must be a multiple of CPU_COUNT
void fillBuffer(uint64_t* data, uint64_t startingBlock, uint32_t blockCount, uint8_t spawnThreads) {
    if(spawnThreads) {
        pthread_t threads[CPU_COUNT];
        struct ThreadData threadData[CPU_COUNT];

        const uint32_t blocksPerThread = blockCount / CPU_COUNT;

        for(uint16_t i = 0; i < CPU_COUNT; i++) {
            threadData[i].startingBlock = startingBlock + blocksPerThread * i;
            threadData[i].blockCount = blocksPerThread;
            threadData[i].data = data;
            threadData[i].arrayOffset = blocksPerThread * i;
            pthread_create(&threads[i], NULL, threadRoutine, &threadData[i]);
        }

        for(uint16_t i = 0; i < CPU_COUNT; i++) {
            pthread_join(threads[i], NULL);
        }

        return;
    }

    for(uint64_t i = 0; i < blockCount; i++) {
        const uint64_t absolutePosition = startingBlock + i;

        uint64_t block = liouvilleBlockLookup(absolutePosition * 64);        
        data[i] = block;
        if(absolutePosition < TAIL_TABLE_SIZE) {
            tailTable[absolutePosition] = block;
        }
    }
}

int main() {
    printStart();

    headTableDivisor = malloc(HEAD_TABLE_SIZE * sizeof(uint16_t));
    headTableFactor = malloc(HEAD_TABLE_SIZE * sizeof(uint8_t));

    for(uint16_t i = 0; i < HEAD_TABLE_SIZE; i++) {
        headTableDivisor[i] = gcdU16(HEAD_TABLE_SIZE, i);
        headTableFactor[i] = liouville_full(headTableDivisor[i]);
    }
    printTailTable();


    primeBitsU32 = primesU32cprs();
    printPrimesTable(bigPIcprs(primeBitsU32, PRIME_BIT_TABLE_LENGTH), PRIME_TABLE_PRIME_COUNT);

    tailTable = malloc(TAIL_TABLE_SIZE * sizeof(uint64_t));

    uint64_t blockCount = 0;

    const uint32_t bufferChunkSize = CPU_COUNT * 16384;
    uint64_t* aggregationBuffer = malloc(bufferChunkSize * sizeof(uint64_t)); 

    printStarting();

    // for(i = 0; ; i++) {  
    for(uint64_t i = 0; i < 15625000; i += bufferChunkSize) {  
        fillBuffer(aggregationBuffer, i, bufferChunkSize, i != 0);
        blockCount++;
        printBlocksFulfilled(blockCount, bufferChunkSize, i != 0);
        manageBuffer(aggregationBuffer, i, bufferChunkSize);
    }

    free(aggregationBuffer);
    free(headTableDivisor);
    free(headTableFactor);
    free(tailTable);
    free(primeBitsU32);

    printEnd();
    return 0;
}
