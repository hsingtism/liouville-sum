#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>

#include <math.h>

// ! do not edit or you might break something
#define TABLE_FACTORIAL_BASE 8
#define HEAD_TABLE_SIZE 40320

#define TAIL_TABLE_SIZE (uint64_t)1000000000
#define TAIL_TABLE_FIRST_ENTRY 0xa835be21f89e39ac // see tailTableForstEntry.c

#define CPU_COUNT 32

uint16_t* headTableDivisor;
uint8_t* headTableFactor;
uint64_t* tailTable;

uint8_t* u32PrimeFlagTable;
uint8_t* primesU32();
int64_t bigPI(uint8_t* table, uint64_t length);

uint8_t liouvilleLookup(uint64_t n) {
    
    const uint64_t initial = n;
    uint8_t factorCount = 0;

    const uint16_t trailingZeroCount = __builtin_ctz(n);
    factorCount ^= trailingZeroCount & 1;
    n = n >> trailingZeroCount;

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)(tailTable[n / 64] >> (n % 64)) & 1;
    }

    factorCount ^= headTableFactor[n % HEAD_TABLE_SIZE];
    n /= headTableDivisor[n % HEAD_TABLE_SIZE];

    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)(tailTable[n / 64] >> (n % 64)) & 1;
    }

    while(n % 3 == 0) {
        factorCount ^= 1;
        n /= 3;
    }
    
    if(n < initial && n < TAIL_TABLE_SIZE * 64) {
        return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
    }

    /* 
        Checking for prime here lets it so the prgram doesn't have to trial division the 
        rest intil sqrt n. The table now only works until 4 billion. so 
    */
    // TODO extend this table somehow
    if(u32PrimeFlagTable[n] && n < UINT32_MAX) {
        return factorCount ^ 1;
    }

    // TODO use primes table here too
    // for(uint32_t div = 6; div <= (uint32_t)sqrt(n) + 1; div += 6) {
    for(uint32_t div = 6; div <= n / (div - 1) + 1; div += 6) {
        while(n % (div - 1) == 0) {
            factorCount ^= 1;
            n /= div - 1;
        }

        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
        }

        while(n % (div + 1) == 0) {
            factorCount ^= 1;
            n /= div + 1;
        }

        if(n < initial && n < TAIL_TABLE_SIZE * 64) {
            return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
        }

    }



    // for(uint32_t div = 5; div <= n / div + 1; div++) {
    //     if(u32PrimeFlagTable[div] == 0) continue;

    //     while(n % div == 0) {
    //         factorCount ^= 1;
    //         n /= div;
    //     }

    //     if(n < initial && n < TAIL_TABLE_SIZE * 64) {
    //         return factorCount ^ (uint8_t)((tailTable[n / 64] >> (n % 64)) & 1);
    //     }

    // }
 
    // case where the number is prime after the 2 and 3 test
    // TODO pretty sure this is not needed now
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

int main() {
    printf("program started. current time: %jd\n", (intmax_t)time(NULL));
    
    // TODO idea: generalized 6k+-1 but with 8! instead of 3!
    headTableDivisor = malloc(HEAD_TABLE_SIZE * sizeof(uint16_t));
    headTableFactor = malloc(HEAD_TABLE_SIZE * sizeof(uint8_t));
    
    headTableDivisor[0] = HEAD_TABLE_SIZE;
    headTableFactor[0] = 1; // liouville_full(HEAD_TABLE_SIZE) is 1

    for(uint16_t i = 1; i < HEAD_TABLE_SIZE; i++) {
        uint16_t workingI = i;
        
        uint16_t divisor = 1;
        uint8_t factors = 0;

        for(uint8_t d = 2; d < TABLE_FACTORIAL_BASE; d++) {
            if(d == 4 || d == 6) continue; // primes only
            if(workingI % d) continue;
            divisor *= d;
            factors ^= 1;
        }

        headTableFactor[i] = factors;
        headTableDivisor[i] = divisor;
    }
    printf("tail table done. current time: %jd\n", (intmax_t)time(NULL));

    u32PrimeFlagTable = primesU32();
    printf("prime flags done with %lu primes. current time: %jd\n", bigPI(u32PrimeFlagTable, UINT32_MAX), (intmax_t)time(NULL));
    printf("the table must contain exactly 203280221 primes\n");

    tailTable = malloc(TAIL_TABLE_SIZE * sizeof(uint64_t));

    int64_t sum = -1; // start at -1 to account for liouville_full(0)
    uint64_t blockCount = 0;

    const uint32_t bufferChunkSize = CPU_COUNT * 16384;
    uint64_t* aggregationBuffer = malloc(bufferChunkSize * sizeof(uint64_t)); 

    printf("all memory allocated. current time: %jd\n", (intmax_t)time(NULL));

    // for(uint64_t i = 0; ; i++) {  
    for(uint64_t i = 0; i < 15625000; i++) {  
        if(i % bufferChunkSize == 0) {
            // cannot multithread on first round because table isn't built
            fillBuffer(aggregationBuffer, i, bufferChunkSize, i != 0);
            blockCount++;

            printf("block %lu with %u values fulfilled. multithreaded: %u. current time: %jd\n",
                blockCount, bufferChunkSize * 64, i != 0, (intmax_t)time(NULL));
        }

        uint64_t block = aggregationBuffer[i % bufferChunkSize];
        sum += __builtin_popcountll(block) * -2 + 64;

        if((i - 1) % (1 << 20) == 0 || abs(sum) < 128) {
            printf("%lu %ld %lx\n", i * 64 + 63, sum, block);
        }
    }
    printf("%ld\n", sum);

    free(aggregationBuffer);
    free(headTableDivisor);
    free(headTableFactor);
    free(tailTable);
    free(u32PrimeFlagTable);

    return 0;
}
