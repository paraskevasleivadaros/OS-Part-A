#!/usr/bin/env bash
gcc p3150173-p3150090-res1.c -o main -lpthread -w
echo -----------------------------------------
./main 100 1000
echo -----------------------------------------
./main 75 1000
echo -----------------------------------------
./main 50 1000
echo -----------------------------------------
./main 25 1000
echo -----------------------------------------
./main 10 1000
echo -----------------------------------------
./main 0 1000
echo -----------------------------------------