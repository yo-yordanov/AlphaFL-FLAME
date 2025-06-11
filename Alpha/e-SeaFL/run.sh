#!/bin/bash

# 62006 274442 817872 100000

REP=4

# e-SeaFL
LOG="logs/eseafl/"
mkdir -p $LOG
for r in $(seq 1 $REP); do

    for INP in 62006; do
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = $INP" AssistingNodeCode/AssistingNodeCode.py
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = $INP" ServerCode/Server.py
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = $INP" UserCode/User.py
        ./script.sh -u 4 -a 3 -c 1 -o 2 -b 1 -p 10050 >>$LOG/"e-SeaFL_$INP""_4.log" 2>&1
    done

    for N in 10 20 30 40; do
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = 100000" AssistingNodeCode/AssistingNodeCode.py
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = 100000" ServerCode/Server.py
        sed -i "/^WEIGHTLISTSIZE = /c\WEIGHTLISTSIZE = 100000" UserCode/User.py
        ./script.sh -u $N -a 3 -c 1 -o 2 -b 1 -p 10050 >>$LOG/"e-SeaFL_100000_$N.log" 2>&1
    done

done

