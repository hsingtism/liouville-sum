#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>

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

void zero(uint8_t* table, uint64_t length) {
    for(uint64_t i = 0; i < length; i++) {
        table[i] = 0;
    }
}

void invert(uint8_t* table, uint64_t length) {
    for(uint64_t i = 0; i < length; i++) {
        table[i] = table[i] == 0;
    }
}

int64_t bigPI(uint8_t* table, uint64_t length) {
    uint64_t sum = 0;
    for(uint64_t i = 0; i < length; i++) {
        sum += table[i];
    }

    return sum;
}

struct ThreadData {
    uint8_t* table;
    uint8_t* primesFlag;
    uint16_t threadIdent;
};

void* threadRoutine(void* arguments) {
    struct ThreadData args = *(struct ThreadData*) arguments;

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
        pthread_create(&threads[i], NULL, threadRoutine, &threadData[i]);
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
    uint8_t* flagsU16 = malloc(UINT16_MAX);
    zero(flagsU16, UINT16_MAX);
    sieve(flagsU16, UINT16_MAX);
    invert(flagsU16, UINT16_MAX);

    uint8_t* flagsU32 = malloc(UINT32_MAX);
    zero(flagsU32, UINT32_MAX);
    u32SieveHelped(flagsU32, flagsU16);
    invert(flagsU32, UINT32_MAX);

    free(flagsU16);
    return flagsU32;
}

int main() {
    uint8_t* flags = primesU32();

    // bigPI(flags, UINT32_MAX) should be exactly 203280221
    printf("prime flags done with %lu primes\n", bigPI(flags, UINT32_MAX));

    return 0;
}