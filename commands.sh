#time ./test
#./test > output.txt
#pmap #PID

#!/bin/bash

set -x
pwd

du output.txt test -h

wc -l output.txt

grep "max" output.txt -c
grep "max" output.txt | head
grep "max" output.txt | tail
grep "min" output.txt -c
grep "min" output.txt | head
grep "min" output.txt | tail
grep "zero" output.txt -c
grep "zero" output.txt | head
grep "zero" output.txt | tail
grep "block" output.txt -c
grep "block" output.txt | head
grep "block" output.txt | tail

grep "primes" output.txt

cat output.txt | head
cat output.txt | tail

echo