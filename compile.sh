#!/bin/bash
gcc -O3 bitfieldHelp.c bitfieldHelp.h entry.c maths.c primes.c primes.h -o test -lm
