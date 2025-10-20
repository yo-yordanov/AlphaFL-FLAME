for INP in 62006 274442 817872; do
    bash Alpha/benchmark.sh -b flame-private -c 4 -i $INP -m $MODE -r 5
done

for INP in 100000 300000 500000; do
    bash Alpha/benchmark.sh -b flame-private -c 10 -i $INP -r 5
done

for CLI in 20 30 40 50; do
    bash Alpha/benchmark.sh -b flame-private -c $CLI -i 100000 -r 5
done

# # ------------------------------------------------------------------------

for INP in 62006 274442 817872; do
    bash Alpha/benchmark.sh -b flame-leaky -c 4 -i $INP -m $MODE -r 5
done

for INP in 100000 300000 500000; do
    bash Alpha/benchmark.sh -b flame-leaky -c 10 -i $INP -r 5
done

for CLI in 20 30 40 50; do
    bash Alpha/benchmark.sh -b flame-leaky -c $CLI -i 100000 -r 5
done
# ------------------------------------------------------------------------

source venv/bin/activate
python Alpha/log_pets25/parse_flame_private.py
python Alpha/log_pets25/parse_flame_leaky.py
