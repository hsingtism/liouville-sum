# Analysis and results computed by me are hosted [here](https://hsing.org/liouville-lambda).

The code is pretty much finalized. In the future, I may also implement an optimized version for L(n) at a specific spot. 

# Multithreaded Evaluator of the Summatory Liouville Function

This program evalulates the summatory liouville function in blocks of 64 values with two lookup tables. It supports multithreading. 

This was one of my first programming projects in 2021, it has since been greatly improved.

Without any tuning, it can sum to 1 billion (where it disproved the [Polya conjecture](https://en.wikipedia.org/wiki/P%C3%B3lya_conjecture)) in about ~~135~~ ~~44~~ 32 seconds on my laptop with an Intel Core Ultra 7 155H under Windows 11 WSL. (Note that the 32 second time is not optimized for the 1 billion run because it allocates a 64 billion value lookup table and a 2<sup>32</sup> long primes table. It does not include the 14 seconds it takes to compress the primes table and the overhead to blocks bit by bit, it uses the single popcount instruction.) It also sums to 10 billion in 897 seconds. 

See [A002819](https://oeis.org/A002819) on the OEIS. I am not familiar enough with the concept to explain the mathmatical part of it.

**Note:** This program, as you will see below, is designed to evalulate all values of lambda up to a value. If you just want the partial sum at a specific large integer, there are tricks to skip values and speed everything up to less than O(n).

## Primality test

This program uses a table generated with sieve of eratosthenes for numbers up to 2<sup>32</sup>. For values larger than that, up to 2<sup>64</sup>, it uses a function described in [this paper](https://ceur-ws.org/Vol-1326/020-Forisek.pdf) and made avilable [here](https://people.ksp.sk/~misof/primes/). The code is copied to `largePrimes.c` with minor modifications. This function is way faster than trial division and with some optimizations, it is slightly slower than the array lookup for values about 10 billion. 

## Compiling

Set settings at `parameters.h`. Note that

- `pthreads` is required. Although multithreading can be easily disabled by removing a few blocks of code.

- `__builtin_popcountll` and `__builtin_ctz` from GCC is used. These can be rewritten quite easily if you're using another compiler.

To compile and run

```
./compile.sh
./liouville > output.txt # or use tee
./inspect.sh
```

## Memory requirements

**around 9 GB needed** unless you modify something

8 GB is allocated for the "tail table" and about 537 MB is allocated for the primes table. The tail table size can be easily decreased, but this will impact performance at large values. The two big tables combined with the stacks shouldn't take more than 8.6 GB.

If you have to use virtual memory, unless your access times is less than a few microseconds, it'd be slower than a few extra divisions. 

# Interpreting output logs

The program will write to `stdout` via `printf`, no matter what you do with it (`>` or `tee`) the output will start like this

```
1722493430: program started
1722493430: head table done
1722493446: prime flags done - 203280221 primes
the table must contain exactly 203280221 primes
1722493446: all memory allocated
1722493451: block 1 with 524288 values fulfilled. multithreaded: 0
1 1 max
1 repeat0 1 1
2 0 zero
2 repeat1 1 2
3 -1 min
3 repeat1 2 2
4 0 zero
6 0 zero
8 -2 min
10 0 zero
10 repeat0 2 9
13 -3 min
13 repeat1 3 11
16 0 zero
```

```
...
1722493452: block 2 with 524288 values fulfilled. multithreaded: 1
...
1722493454: block 3 with 524288 values fulfilled. multithreaded: 1
... not in order
100000000 -3884 periodic
...
200000000 -11126 periodic
```

and end with

```
1722493499: peaceful termination
```

if it runs until to the end.

Here is how to read the log file.

## Status messages

These aren't actual data. They're started with the time, with the exception of `the table must contain exactly 203280221 primes`. They mean the following

- `program started` prints as soon as the program starts
- `head table done` usually takes less than a second. This is the 30030 large factor wheel.
- `prime flags done` means the prime number table is completed. This takes about 15 seconds. The prime number table contains all values less than 2<sup>32</sup>. The number primes is also printed out. This must match the provided &pi;(2<sup>32</sup>) value.
- `all memory allocated` indicates that all preparation is done and the main loop is going to start. If any `malloc` call fails, the program will crash soon.
- `block x fulfilled` indicates the buffer is filled. The number of "values" is the number of 64 bit blocks, each bit is one n of the function. It also states if the block is multithreaded when computing. Only the first block is not multithreaded because the tail table needs to be filled.
- `peaceful termination` when the programs stops at the end of the loop

## Mathmatical data

### `max`, `min`, `periodic`, and `zero`

Theese follow the format

```
[n] [liouville_sum(n)] [flag]
```

`liouville_sum` means the summatory Liouville function (OEIS [Partial sum of Liouville function](https://oeis.org/A002819)). The following flags exists

- `max` the function is at all time maximum (OEIS [Maximum records](https://oeis.org/A051470))
- `min` the function is at all time minimum (OEIS [Minimum records](https://oeis.org/A002053))
- `periodic` periodic value print out, usually once every 100000000 (OEIS [Partial sum of Liouville function](https://oeis.org/A002819) and [Evalulated every 10<sup>n</sup>](https://oeis.org/A090410))
- `zero` the function is zero (OEIS [Zeros](https://oeis.org/A028488))

### `repeat0` and `repeat1`

These are record intervals in which 
- the Liouville function is the same value
- the summatory Liouville function is strictly increasing or decreasing

`repeat0` is repeats of 1, or increasing; `repeat1` is repeats of -1, or decreasing. Note L(n) = (-1)<sup>&Omega;(n)</sup>.

The log entries are in the format

```
[n] [repeat0 or repeat1] [n at which the repeat started]
```
See OEIS [record 1's](https://oeis.org/A175201) for `repeat0` and [record -1's](https://oeis.org/A175202) for `repeat1`.

# Useful OEIS entries

some are listed above

[Liouville function](https://oeis.org/A008836)

[Partial sum of Liouville function](https://oeis.org/A002819)

## of the summatory Liouville function

[Zeros](https://oeis.org/A028488)

[Minimum records](https://oeis.org/A002053)

[Maximum records](https://oeis.org/A051470)

[Evalulated every 10<sup>n</sup>](https://oeis.org/A090410)

## of the Liouville function

[record 1's](https://oeis.org/A175201)

[record -1's](https://oeis.org/A175202)

## prime number behaviors

[Primorials](https://oeis.org/A002110)

[pi(2<sup>n</sup>)](https://oeis.org/A007053)
