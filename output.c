#include "output.h"
#include "bitfieldHelp.h"

#include <time.h>

void manageBuffer(uint64_t* buffer, uint64_t position, uint32_t blocks) {

    static int64_t sum = 0;
    static int64_t minVal = 0;
    static int64_t maxVal = 0;

    static uint32_t repeatLength = 0;
    static uint8_t repeatEntry = 0; 
    static uint32_t repeatRecord0 = 0;
    static uint32_t repeatRecord1 = 0;

    for(uint64_t i = 0; i < blocks * 64; i++) {
        uint64_t absolutePosition = i + position * 64;
        if(absolutePosition == 0) continue;
        
        uint8_t value = getBit(buffer, i);

        sum += value * -2 + 1;

        if(absolutePosition % 100000000 == 0) {
            printf("%lu %ld periodic\n", absolutePosition, sum);
        }

        if(sum == 0) {
            printf("%lu %ld zero\n", absolutePosition, sum);
        }

        if(sum < minVal) {
            minVal = sum;
            printf("%lu %ld min\n", absolutePosition, sum);
        }

        if(sum > maxVal) {
            maxVal = sum;
            printf("%lu %ld max\n", absolutePosition, sum);
        }

        repeatLength *= repeatEntry == value;
        repeatEntry = value; 
        repeatLength++;

        if(repeatEntry == 0 && repeatLength > repeatRecord0) {
            repeatRecord0 = repeatLength;
            printf("%lu repeat0 %u %lu\n", absolutePosition, repeatLength, absolutePosition - repeatLength + 1);
        } else if(repeatEntry == 1 && repeatLength > repeatRecord1) {
            repeatRecord1 = repeatLength;
            printf("%lu repeat1 %u %lu\n", absolutePosition, repeatLength, absolutePosition - repeatLength + 1);
        }
    }

}

void printStart() {
    printf("%jd: program started\n", (intmax_t)time(NULL));
}

void printTailTable() {
    printf("%jd: tail table done\n", (intmax_t)time(NULL));
}

void printPrimesTable(uint64_t bigPi, uint64_t expectedPi) {
    printf("%jd: prime flags done - %lu primes \n", (intmax_t)time(NULL), bigPi);
    printf("the table must contain exactly %lu primes\n", expectedPi);

}

void printStarting() {
    printf("%jd: all memory allocated\n", (intmax_t)time(NULL));
}

void printBlocksFulfilled(uint64_t blockCount, uint32_t chunkSize, uint8_t multithreaded) {
    printf("%jd: block %lu with %u values fulfilled. multithreaded: %u\n",
        (intmax_t)time(NULL), blockCount, chunkSize, multithreaded);
}

void printEnd() {
    printf("%jd: peaceful termination\n", (intmax_t)time(NULL));
}
