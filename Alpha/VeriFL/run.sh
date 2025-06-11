#!/bin/bash

# 62006 274442 817872 100000

REP=4

# VeriFL
LOG="logs/verifl/"
mkdir -p $LOG
for r in $(seq 1 $REP); do
    for INP in 62006; do
        java -Xmx8192m -Xms8192m -Xss8192m -jar Server.jar -logR 24 -d $INP -N 4 -t 2 -batch 1 -maskedColl 0,0 -decom 0 >>$LOG/"VeriFL_$INP""_4.log" 2>&1
    done

    for N in 10 20 30 40; do
        java -Xmx8192m -Xms8192m -Xss8192m -jar Server.jar -logR 24 -d 100000 -N $N -t $((N/2)) -batch 1 -maskedColl 0,0 -decom 0 >>$LOG/"VeriFL_100000_$N.log" 2>&1
    done
done