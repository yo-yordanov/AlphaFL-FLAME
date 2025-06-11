#!/usr/bin/env bash

# benchmark Alpha input commitment
for MODE in passive active; do
    for CLI in 1 4; do
        for INP in 62006 274442 817872; do
            bash Alpha/benchmark.sh -b alpha-input -c $CLI -i $INP -m $MODE -r 5
        done
    done
done

for MODE in passive active; do
    for CLI in 10 20 30 40; do
        for INP in 100000 300000; do
            bash Alpha/benchmark.sh -b alpha-input -c $CLI -i $INP -m $MODE -r 5
        done
    done
done

# benchmark Alpha secure aggregation
for CLI in 4; do
    for INP in 62006 274442 817872; do
        bash Alpha/benchmark.sh -b alpha-sa -c $CLI -i $INP -r 5
    done
done

for CLI in 10 20 30 40; do
    for INP in 100000 300000; do
        bash Alpha/benchmark.sh -b alpha-sa -c $CLI -i $INP -r 5
    done
done