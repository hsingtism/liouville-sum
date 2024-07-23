# This is being reworked on

This project was one of my first programming projects. After two years, I am reworking it. The original code will be placed in `/naive` and I will now try different (faster) approaches. This repo is actively being worked on (see recent commits).

# Multithreaded Evaluator of the Summatory Liouville Function

This program evalulates the summatory liouville function in blocks of 64 values with two lookup tables. It supports multithreading. Without any tuning, it can sum to 1 billion (where it disproved the [Polya conjecture](https://en.wikipedia.org/wiki/P%C3%B3lya_conjecture)) in about ~~135~~ ~~44~~ 32 seconds on my laptop with an Intel Core Ultra 7 155H under Windows 11 WSL. 

See [A002819](https://oeis.org/A002819) on the OEIS. I am not familiar enough with the concept to explain the mathmatical part of it.

## Compiling

`pthreads` is required. Although multithreading can be easily disabled by removing the block in `fillbuffer` and support functions.

`__builtin_popcountll` and `__builtin_ctz` from GCC is used. These can be rewritten quite easily if you're using another compiler.

`compile.sh` has the compilation command.

## (CURRENTLY OUTDATED) Analysis and results computed by me are hosted [here](https://hsing.org/liouville-sum).