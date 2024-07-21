# Summatory liouville function evaluator

## This is being reworked on

This project was one of my first programming projects. After two years, I am reworking it. The original code will be placed in `/naive` and I will now try different (faster) approaches. The web page is still outdated, I will be updated when I'm done.

## Original description

This is a program that evaulates the value of the summatory liouville function given a point. Change the defines at the beginning to start evaluation from another place.

The summatory liouville function is determined by the liouville function, which is -1 one when a number has an odd number of prime factors and +1 when even. For example the liouville function of 20 would be -1 since 20 has 3 prime factors. The summatory function basically sums the liouville function up to the input. 

This program uses the brute-force way of evaluating the functions, outputting the sum every set amount of numbers for use. Its time complexity (where the input is the evaluation target) is a little faster than O(n sqrt n) experimentally for values to about 1 billion

This program can evalulate values up to 2^51 - 1, `sqrt()` does not behave properly after that. Note the max value of the data type of `divis` in `liouvFrag()` the square root of `i`. 

At about 906,150,257, a counterexample of the polya conjecture is shown.

If you're not using GCC. You will need to change the ctz function.

## Analysis and results computed by me are hosted [here](https://hsing.org/liouville-sum).