# This is being reworked on

This project was one of my first programming projects. After two years, I am reworking it. The original code will be placed in `/naive` and I will now try different (faster) approaches. This repo is actively being worked on (see recent commits).

As of now, the time it takes to evaluate 10 billion values has been improved from 406974 seconds to 897 seconds. (The first time was on worse hardware.)

# Multithreaded Evaluator of the Summatory Liouville Function

This program evalulates the summatory liouville function in blocks of 64 values with two lookup tables. It supports multithreading. Without any tuning, it can sum to 1 billion (where it disproved the [Polya conjecture](https://en.wikipedia.org/wiki/P%C3%B3lya_conjecture)) in about ~~135~~ ~~44~~ 32 seconds on my laptop with an Intel Core Ultra 7 155H under Windows 11 WSL. (Note that the 32 second time is not optimized for the 1 billion run because it allocates a 64 billion value lookup table and a 2<sup>32</sup> long primes table. It also down not include the 14 seconds it takes to compress the primes table, which reduce memory usage but hurts time.) It also sums to 10 billion in 897 seconds. 

See [A002819](https://oeis.org/A002819) on the OEIS. I am not familiar enough with the concept to explain the mathmatical part of it.

## Primality test

This program uses a table generated with sieve of eratosthenes for numbers up to 2<sup>32</sup>. For values larger than that, up to 2<sup>64</sup>, it uses a function described in [this paper](https://ceur-ws.org/Vol-1326/020-Forisek.pdf) and made avilable [here](https://people.ksp.sk/~misof/primes/). The code is copied to `largePrimes.c` with minor modifications. This function is way faster than trial division and with some optimizations, it is slightly slower than the array lookup for values about 10 billion. 

## Compiling

`pthreads` is required. Although multithreading can be easily disabled by removing a few blocks of code.

`__builtin_popcountll` and `__builtin_ctz` from GCC is used. These can be rewritten quite easily if you're using another compiler.

`compile.sh` has the compilation command.

## Memory requirements

**around 9 GB needed** unless you modify something

8 GB is allocated for the "tail table" and about 537 MB is allocated for the primes table. The tail table size can be easily decreased, but this will impact performance at large values. The two big tables combined with the stacks shouldn't take more than 8.6 GB.

If you have to use virtual memory, unless your access times is less than a few microseconds, it'd be slower than a few extra divisions. 

## (CURRENTLY OUTDATED) Analysis and results computed by me are hosted [here](https://hsing.org/liouville-sum).
