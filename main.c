#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

// define starting para
#define startingN 1                  // log file **PLUS 1** or 1
#define startingSum 0                // log file or 0
#define endingN 9007199254740991     // default is 9007199254740991 - max value for double to behave
#define printIntv 10000000           // default is 10000000

long liouvFrag(uint64_t seed) {
    char pfCount = 0;

    int tz = __builtin_ctz(seed);  // needs GCC
    pfCount += tz;
    seed = seed >> tz;

    while (seed % 3 == 0) {
        pfCount++;
        seed *= 0xAAAAAAAAAAAAAAAB;  // divide by 3
    }

    for (unsigned long divis = 6; divis <= sqrt(seed) + 1; divis += 6) {
        divis--;
        while (seed % divis == 0) {
            pfCount++;
            seed /= divis;
        }
        divis += 2;
        while (seed % divis == 0) {
            pfCount++;
            seed /= divis;
        }
        divis--;
    }
    if (seed > 2) {
        pfCount++;
    }
    return 1 - (pfCount % 2) * 2;
}

int main() {
    long liouvSum = startingSum;
    for (uint64_t i = startingN; i < endingN; i++) {
        liouvSum += liouvFrag(i);
        if (liouvSum == 0 || i % printIntv == 0) {
            printf("%llu: %ld\n", i, liouvSum);
            FILE *fp;
            fp = fopen("computed.txt", "a");
            fprintf(fp, "%lu - %llu: %ld\n", (unsigned long)time(NULL), i, liouvSum);
            fclose(fp);
        }
    }
    return 0;
}