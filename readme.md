# Summatory liouville function evaluator

## better known for its connection with the polya conjecture

This is a program that evaulates the value of the summatory liouville function given a point. Change the consts at the beginning to start eval from another place.

The summatory liouville function is determined by the liouville function, which is -1 one when a number has an odd number of prime factors and +1 when even. For example the liouville function of 20 would be -1 since 20 has 3 prime factors. The summatory function basically sums the liouville function up to the input. 

This program uses the brute-force way of evaluating the functions, outputting the sum every set amount of numbers for use. Its time complexity (where the input is the evaluation target) is a little faster than O(n sqrt n) experimentally for values to about 1 billion

This program can evalulate values up to ULLONG_MAX, or 18,446,744,073,709,551,615. Data types needs to be changed if larger numbers needs to be evaluated. Note the max value of the data type of `divis` in `liouvFrag()` the square root of `i`. 

If you're not using GCC. You will need to change the ctz function.

## Analysis and results computed by me are hosted [here](https://hsing.org/liouville-sum).