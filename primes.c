#include "primes.h"
#include "bitfieldHelp.h"

#define PRIME_TABLE_THREAD_COUNT 32

void sieve(uint8_t* table, uint64_t length) {
    table[0] = 1;
    table[1] = 1;
    for(uint64_t i = 2; i < length; i++) {
        if(table[i] == 1) continue;
        for(uint64_t j = i * 2; j < length; j += i) {
            table[j] = 1;
        }
    }
}

void invert(uint8_t* table, uint64_t length) {
    for(uint64_t i = 0; i < length; i++) {
        table[i] = table[i] == 0;
    }
}

// int64_t bigPI(uint8_t* table, uint64_t length) {
//     uint64_t sum = 0;
//     for(uint64_t i = 0; i < length; i++) {
//         sum += table[i];
//     }

//     return sum;
// }

struct ThreadData {
    uint8_t* table;
    uint8_t* primesFlag;
    uint16_t threadIdent;
};

void* primeThreadRoutine(void* arguments) {
    struct ThreadData args = *(struct ThreadData*) arguments;

    // these increment then skip is extremely fast so no need to further optimize
    // not perfect parallelism because smaller threadIdent will have way more tasks
    for(uint64_t i = args.threadIdent; i < UINT16_MAX; i += PRIME_TABLE_THREAD_COUNT) {
        if(args.primesFlag[i] == 0) continue;
        for(uint64_t j = i * 2; j < UINT32_MAX; j += i) {
            args.table[j] = 1; // the race condition is idempotent
        }
    }

    return NULL;
}

void u32SieveHelped(uint8_t* table, uint8_t* primesFlag) {
    table[0] = 1;
    table[1] = 1;
    
    pthread_t threads[PRIME_TABLE_THREAD_COUNT];
    struct ThreadData threadData[PRIME_TABLE_THREAD_COUNT];

    for(uint16_t i = 0; i < PRIME_TABLE_THREAD_COUNT; i++) {
        threadData[i].table = table;
        threadData[i].primesFlag = primesFlag;
        threadData[i].threadIdent = i;
        pthread_create(&threads[i], NULL, primeThreadRoutine, &threadData[i]);
    }

    for(uint16_t i = 0; i < PRIME_TABLE_THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // single threaded version
    // for(uint64_t i = 0; i < UINT16_MAX; i++) {
    //     if(primesFlag[i] == 0) continue;
    //     for(uint64_t j = i * 2; j < UINT32_MAX; j += i) {
    //         table[j] = 1;
    //     }
    // }
}

uint8_t* primesU32() {
    uint8_t* flagsU16 = calloc(UINT16_MAX, sizeof(uint8_t));
    sieve(flagsU16, UINT16_MAX);
    invert(flagsU16, UINT16_MAX);

    uint8_t* flagsU32 = calloc(UINT32_MAX, sizeof(uint8_t));
    u32SieveHelped(flagsU32, flagsU16);
    invert(flagsU32, UINT32_MAX);

    free(flagsU16);
    return flagsU32;
}

// ! DO NOT USE "UINT_32_MAX / 64"
// using the type_MAX constant misses one value, but that value is always
// composite. using it in the bit table misses 2 primes.

// converting from the byte flag to the bit flag takes about 5-10 seconds
// but it saves 7/8 memory. Accessing does not add a lot of overhead.
// TODO maybe find a fast way to do a factor wheel check and map
// TODO for higher values maybe have a list and do binary search
// we do need a big initial prime table because a test takes a long time
// and the nature of the program will benefir from any bigger table
// as of now, `liouvilleLookup` is effectively doing trial division
// which will get very slow as n increases. 

// this is done singlethreaded else it might lead to a race condition

uint64_t* primesU32cprs() {
    uint8_t* flagsU32 = primesU32();
    uint64_t* flagsU32cprs = calloc(PRIME_BIT_TABLE_LENGTH, sizeof(uint64_t));

    for(uint64_t i = 0; i < UINT32_MAX; i++) {
        setBitFromZero(flagsU32cprs, i, flagsU32[i]);
    }
    
    free(flagsU32);
    return flagsU32cprs;
}

int64_t bigPIcprs(uint64_t* table, uint64_t blockLength) {
    uint64_t sum = 0;
    for(uint64_t i = 0; i < blockLength; i++) {
        sum += __builtin_popcountll(table[i]);
    }

    return sum;
}
