#include <stdio.h>
#include <math.h>
// #include <stdint.h>

int liouvFrag(unsigned long long seed)
{
    int pfCount = 0;
    while (seed % 2 == 0) {
        pfCount++;
        seed = seed >> 1;
    }
    for (unsigned long divis = 3; divis <= sqrt(seed); divis = divis + 2) { //it's faster to call sqrt() than squre a 128uint 
        while (seed % divis == 0) {
            pfCount++;
            seed /= divis;
        }
    }
    if (seed > 2)
    {
        pfCount++;
    }

    return 1 - (pfCount % 2) * 2;
}

int main()
{
    long liouvSum = 0;
    for (unsigned long long i = 1; i < 10000000; i++)
    {
        liouvSum += liouvFrag(i);
        // printf("%lu: %ld\n",i,liouvSum);
        if (liouvSum == 0 || i % 1000000 == 0)
        {
            printf("%lu: %ld\n", i, liouvSum);
        }
    }
    return 0;
}
