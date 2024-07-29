#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>

#include <math.h>

#include "bitfieldHelp.h"
#include "primes.h"

uint64_t* primeBitsU32;
uint8_t millerRabinU64(uint64_t n);

// https://oeis.org/A002110
#define HEAD_TABLE_SIZE 30030
uint16_t gcdU16(uint16_t a, uint16_t b);
uint8_t liouville_full(uint64_t n);

#define TAIL_TABLE_SIZE (uint64_t)1000000000
#define TAIL_TABLE_FIRST_ENTRY 0xa835be21f89e39ac // see tailTableForstEntry.c

#define CPU_COUNT 32

uint16_t* headTableDivisor;
uint8_t* headTableFactor;
uint64_t* tailTable;

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
    } 
    // else if(headTableDivisor[n % HEAD_TABLE_SIZE] != 1 && millerRabinU64(n)) {
    //     return factorCount ^ 1;
    // }
    
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

void printZeros(uint64_t block, int64_t sum, uint64_t offset) {
    int64_t sumF = sum;
    if(sumF == 0) {
        printf("%lu %ld\n", offset, sumF);
    }
    for(uint64_t i = 0; i < 64; i++) {
        sumF += getBit(&block, i) * -2 + 1;
        if(sumF == 0) {
            printf("%lu %ld\n", offset + i, sumF);
        }
    }
}

int main() {
    printf("%jd: program started\n", (intmax_t)time(NULL));
    
    headTableDivisor = malloc(HEAD_TABLE_SIZE * sizeof(uint16_t));
    headTableFactor = malloc(HEAD_TABLE_SIZE * sizeof(uint8_t));

    for(uint16_t i = 0; i < HEAD_TABLE_SIZE; i++) {
        headTableDivisor[i] = gcdU16(HEAD_TABLE_SIZE, i);
        headTableFactor[i] = liouville_full(headTableDivisor[i]);
    }
    printf("%jd: tail table done\n", (intmax_t)time(NULL));


    primeBitsU32 = primesU32cprs();
    printf("%jd: prime flags done - %lu primes \n", (intmax_t)time(NULL), bigPIcprs(primeBitsU32, PRIME_BIT_TABLE_LENGTH));
    printf("the table must contain exactly %u primes\n", PRIME_TABLE_PRIME_COUNT);

    tailTable = malloc(TAIL_TABLE_SIZE * sizeof(uint64_t));

    int64_t sum = -1; // start at -1 to account for liouville_full(0)
    uint64_t blockCount = 0;

    const uint32_t bufferChunkSize = CPU_COUNT * 16384;
    uint64_t* aggregationBuffer = malloc(bufferChunkSize * sizeof(uint64_t)); 

    printf("%jd: all memory allocated\n", (intmax_t)time(NULL));

    // for(uint64_t i = 0; ; i++) {  
    for(uint64_t i = 0; i < 15625000; i++) {  
    // for(uint64_t i = 0; i < 156250000; i++) {  
        if(i % bufferChunkSize == 0) {
            // cannot multithread on first round because table isn't built
            fillBuffer(aggregationBuffer, i, bufferChunkSize, i != 0);
            blockCount++;

            printf("%jd: block %lu with %u values fulfilled. multithreaded: %u\n",
                (intmax_t)time(NULL), blockCount, bufferChunkSize * 64, i != 0);
        }

        uint64_t block = aggregationBuffer[i % bufferChunkSize];

        if(abs(sum) < 64) {
            printZeros(block, sum, i * 64);
        }

        sum += __builtin_popcountll(block) * -2 + 64;

        if((i - 1) % (1 << 20) == 0) {
            printf("%lu %ld %lx\n", i * 64 + 63, sum, block);
        }
    }
    printf("%ld\n", sum);

    free(aggregationBuffer);
    free(headTableDivisor);
    free(headTableFactor);
    free(tailTable);
    free(primeBitsU32);

    return 0;
}
