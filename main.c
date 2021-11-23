#include <stdio.h>
#include <math.h>
#include <time.h>
// #include <stdint.h>

//define starting para
const unsigned long long startingN = 1; //log file **PLUS 1** or 1 
const long startingSum = 0; //log file or 0
const unsigned long long endingN = 9223372036854775807; //default is 9223372036854775807
const unsigned long long printIntv = 10000000; //default is 10000000

long liouvFrag(unsigned long long seed) //since running count uses long
{
    char pfCount = 0;
    int tz = __builtin_ctz(seed); //ctz returns int so
    pfCount += tz;
    seed = seed >> tz;
    while (seed % 3 == 0)
    {
        pfCount++;
        seed *= 0xAAAAAAAAAAAAAAAB; //divide by 3 hack
    }

    for (unsigned long divis = 6; divis <= sqrt(seed)+1; divis += 6)
    { //it's faster to call sqrt() than squre a 128uint
        divis--;
        while (seed % divis == 0)
        {
            pfCount++;
            seed /= divis;
        }
        divis += 2;
        while (seed % divis == 0)
        {
            pfCount++;
            seed /= divis;
        }
        divis--;
    }
    if (seed > 2)
    {
        pfCount++;
    }
    return 1 - (pfCount % 2) * 2;
}

int main()
{
    long liouvSum = startingSum;
    for (unsigned long long i = startingN; i < endingN; i++)
    {
        liouvSum += liouvFrag(i);
        if (liouvSum == 0 || i % printIntv == 0)
        {
            printf("%llu: %ld\n", i, liouvSum);
            FILE *fp;
            fp = fopen("computed.txt", "a");
            fprintf(fp, "%llu - %lu: %ld\n", (unsigned long)time(NULL), i, liouvSum);
            fclose(fp);
        }
    }
    return 0;
}
