#!/bin/bash
gcc -O3 bitfieldHelp.c entry.c maths.c primes.c largePrimes.c -o test -lm -Wall
