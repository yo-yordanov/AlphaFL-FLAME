## AlphaFL (fork)

This fork extends the original AlphaFL artifacts with two additional FLAME secure aggregation variants:
- flame-private
- flame-leaky

Both are run via the same benchmarking interface as alpha-sa, using the -b switch:
- -b flame-private
- -b flame-leaky

Setup remains unchanged and setup.sh is still required.

## Description
Artifacts for Section 7: Evaluation of the paper “AlphaFL: Secure Aggregation with Malicious² Security for Federated Learning against Dishonest Majority” (https://doi.org/10.56553/popets-2025-0134), including:
- Proof-of-concept source code
- Logs for tables and figures
- Baseline benchmarking scripts

## Requirements
- Hardware: 2-core x64 CPU, 8GB RAM for small inputs (≤10 clients, ≤100K parameters). Larger inputs may need ≥128GB RAM.
- OS: Ubuntu 24.04 recommended; Ubuntu 22.04 tested.
- Base: MP-SPDZ v0.3.9 (https://github.com/data61/MP-SPDZ/tree/v0.3.9)

Estimated time and storage:
- Small inputs: ~1 min execution, ~10 min compilation
- Large inputs: ~5 min execution, ~30 min compilation
- Storage: ≥8GB

## Environment Setup
Run the standard setup to install dependencies and prepare directories (Persistence, Player-Data):

````markdown
sudo apt install automake build-essential clang cmake git libboost-dev libboost-all-dev libgmp-dev libntl-dev libsodium-dev libssl-dev libtool libomp-dev python3

git clone https://github.com/Barkhausen-Institut/AlphaFL.git
cd AlphaFL

bash Alpha/setup.sh
````

Optional Python packages for parsing logs and generating figures:
````markdown
# globally
sudo apt install python3-numpy python3-pandas python3-matplotlib

# or in a venv
python3 -m venv venv
source venv/bin/activate
pip3 install numpy pandas matplotlib
````

## Quick Test
Tiny-input smoke tests; logs will appear in Alpha/log_pets25/alpha_input/ and Alpha/log_pets25/alpha_sa/.

````markdown
bash Alpha/benchmark.sh -b alpha-input -c 2 -i 2000 -m passive -r 1
bash Alpha/benchmark.sh -b alpha-sa     -c 2 -i 2000 -r 1
````

## Secure Aggregation Variants
This fork adds FLAME variants in addition to alpha-sa. Run them with the same interface:

- AlphaFL secure aggregation (original):
  ````markdown
  bash Alpha/benchmark.sh -b alpha-sa       -c [clients] -i [inputs] -r [repetitions]
  # example
  bash Alpha/benchmark.sh -b alpha-sa       -c 2 -i 2000 -r 1
  ````

- FLAME (private):
  ````markdown
  bash Alpha/benchmark.sh -b flame-private  -c [clients] -i [inputs] -r [repetitions]
  # example
  bash Alpha/benchmark.sh -b flame-private  -c 2 -i 2000 -r 1
  ````

- FLAME (leaky):
  ````markdown
  bash Alpha/benchmark.sh -b flame-leaky    -c [clients] -i [inputs] -r [repetitions]
  # example
  bash Alpha/benchmark.sh -b flame-leaky    -c 2 -i 2000 -r 1
  ````

Full paper test suite:
````markdown
bash Alpha/run.sh
````

## Parsing Logs and Reproducing Tables/Figures
AlphaFL and FLAME:
````markdown
# alpha_input logs: Alpha/log_pets25/alpha_input/
python3 Alpha/log_pets25/parse_alpha_input_honest.py
python3 Alpha/log_pets25/parse_alpha_input_dishonest.py
# outputs: Alpha/log_pets25/alpha_input_honest.csv, Alpha/log_pets25/alpha_input_dishonest.csv

# alpha_sa logs: Alpha/log_pets25/alpha_sa/
python3 Alpha/log_pets25/parse_alpha_sa.py
# output: Alpha/log_pets25/alpha_sa.csv

# flame-private logs: Alpha/log_pets25/flame_private/
python3 Alpha/log_pets25/parse_flame_private.py
# output: Alpha/log_pets25/flame_private.csv

# flame-leaky logs: Alpha/log_pets25/flame_leaky/
python3 Alpha/log_pets25/parse_flame_leaky.py
# output: Alpha/log_pets25/flame_leaky.csv
````

Baselines:
````markdown
# elsa: Alpha/log_pets25/elsa/
python3 Alpha/log_pets25/parse_elsa.py
# prio: Alpha/log_pets25/prio/
python3 Alpha/log_pets25/parse_prio.py
# verifl: Alpha/log_pets25/verifl/
python3 Alpha/log_pets25/parse_verifl.py
# e-seafl: Alpha/log_pets25/eseafl/
python3 Alpha/log_pets25/parse_eseafl.py
````

Notebook:
- Alpha/log_pets25/pets.ipynb contains code for Table 1–4 and Figures 10–11; microbenchmarks for Tables 5–6 use logs under Alpha/log_pets25/micro_input/ and Alpha/log_pets25/micro_select/.

## Microbenchmarks
Input commitment (MP-SPDZ baseline only; AlphaFL version covered above):
````markdown
bash Alpha/benchmark.sh -b micro-input -i 2000
for INP in 62006 274442 817872; do bash Alpha/benchmark.sh -b micro-input -i $INP; done
````

Silent select:
````markdown
bash Alpha/benchmark.sh -b micro-select -i 2000
for INP in 62006 274442 817872; do bash Alpha/benchmark.sh -b micro-select -i $INP; done
````

## Limitations
- Input Commitment uses low-level primitives (e.g., vOLE) that need further integration work with MP-SPDZ.
- One-server baselines (e.g., RoFL) are difficult to evaluate due to documentation and compatibility issues.

## License
- Code in Alpha/ and Programs/Source/alpha_*.mpc: MIT (see LICENSE).
- MP-SPDZ code adapted from https://github.com/data61/MP-SPDZ/tree/v0.3.9; original Readme.txt and License.txt preserved.

About the assistant: I am gpt-5.