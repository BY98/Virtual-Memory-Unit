#!/bin/bash -e
echo "Compiling"
gcc Part1.c -o Part1
echo "Running"
./Part1 BACKING_STORE.bin addresses.txt > out.txt
echo "Comparing"
diff out.txt correct.txt
