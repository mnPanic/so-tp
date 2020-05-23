#!/bin/bash
make clean
rm test_out.txt

for i in {1..100000}
do 
    echo "exec $i"
    make test >> test_out.txt
done

PASSED = grep "0 failures" test_out.txt | wc -l

echo "passed $PASSED"