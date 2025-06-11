# Artifact Appendix

Paper title: **AlphaFL: Secure Aggregation with Malicious<sup>2</sup> Security for Federated Learning against Dishonest Majority**

Artifacts HotCRP Id: **#21**

Requested Badge: **Available**, **Functional**, **Reproduced**

## Description
This repository contains the artifacts of **Section 7: Evaluation** of our paper.
It includes source code of our proof-of-concept implementations, logs for tables and figures, instructions to benchmark the baselines.

### Security/Privacy Issues and Ethical Concerns (All badges)
No security/privacy issues or ethical concerns.

## Basic Requirements (Only for Functional and Reproduced badges)

### Hardware Requirements
Commodity hardware (2Core x64 CPU and 8GB RAM) can compile and execute the code for small inputs (up to 10 clients and 100K parameters), while more RAMs (128GB) are needed for the larger inputs.

### Software Requirements
- Ubuntu 24.04 with command line is recommended as the OS.
- Ubuntu 22.04 (Compute VM from PETS is also tested, pets-x7e9-3-152-compute.artifacts.measurement.network)

Our implementation is based on [MP-SPDZ v0.3.9](https://github.com/data61/MP-SPDZ/tree/v0.3.9).
You may check the original project for troubleshooting.

### Estimated Time and Storage Consumption
For small inputs, the program could be executed within 1 minute, while the compilation might take 10 minutes.

For large inputs, the program might need 5 minutes for execution and 30 minutes for compilation.

Please have at least 8GB storage for the repository.

## Environment 

### Accessibility (All badges)
Our artifact will be available at [GitHub](https://github.com/Barkhausen-Institut/AlphaFL).

### Set up the environment (Only for Functional and Reproduced badges)
Install dependency and setup the directory.
After these commands, there should be two new subfolders in the repository: *Persistence* and *Player-Data*.
```bash
sudo apt install automake build-essential clang cmake git libboost-dev libboost-all-dev libgmp-dev libntl-dev libsodium-dev libssl-dev libtool libomp-dev python3

git clone https://github.com/Barkhausen-Institut/AlphaFL.git
cd AlphaFL

bash Alpha/setup.sh
```

Several *python* packages are used to parse the logs and generate tables and figures for the papar.
You may install them globally or locally.
```bash
# globally for all users
sudo apt install python3-numpy python3-pandas python3-matplotlib

# locally in a virtual environment
python3 -m venv venv
source venv/bin/activate
pip3 install numpy pandas matplotlib
```

### Testing the Environment (Only for Functional and Reproduced badges)
Perform input commitment and secure aggregation with tiny inputs.
After these two tests, there should be logs at [Alpha/log_pets25/alpha_input/](Alpha/log_pets25/alpha_input/) and [Alpha/log_pets25/alpha_sa/](Alpha/log_pets25/alpha_sa/).
```bash
bash Alpha/benchmark.sh -b alpha-input -c 2 -i 2000 -m passive -r 1
bash Alpha/benchmark.sh -b alpha-sa -c 2 -i 2000 -r 1
```

## Artifact Evaluation (Only for Functional and Reproduced badges)

### Main Results and Claims

#### Main Result 1:
Compared to the state-of-the-art semi-malicious counterpart Elsa, AlphaFL achieves malicious<sup>2</sup> security with slightly more runtime overhead with even less communication cost.

This result is presented in **Section 7.4**.

```bash
# AlphaFL input commitment
# raw logs at *Alpha/log_pets25/alpha_input/*
# parse logs
python3 Alpha/log_pets25/parse_alpha_input_honest.py
python3 Alpha/log_pets25/parse_alpha_input_dishonest.py
# aggregated output *Alpha/log_pets25/alpha_input_honest.csv* and *Alpha/log_pets25/alpha_input_dishonest.csv*

# AlphaFL secure aggregation
# raw logs at *Alpha/log_pets25/alpha_sa/*
# parse logs
python3 Alpha/log_pets25/parse_alpha_sa.py
# aggregated output *Alpha/log_pets25/alpha_sa.csv*

# Elsa
# raw logs at *Alpha/log_pets25/elsa/*
# parse logs
python3 Alpha/log_pets25/parse_elsa.py
# aggregated output *Alpha/log_pets25/elsa.csv*

# Prio
# raw logs at *Alpha/log_pets25/prio/*
# parse logs
python3 Alpha/log_pets25/parse_prio.py
# aggregated output *Alpha/log_pets25/prio.csv*
```

Section **Table 3** and **Table 4** of the python notebook [*Alpha/log_pets25/pets.ipynb*](Alpha/log_pets25/pets.ipynb) show how the csv files are used for the tables in the paper.
The notebook also includes the code to generate **Figure 10** and **Figure 11** in **Section 7.5**.

#### Main Result 2:
AlphaFL is more efficient than one-server solutions, as shown in **Section 7.3**.

```bash
# AlphaFL logs and csv files are the same as before

# RoFL
# raw logs at *Alpha/log_pets25/rofl/*
# runtime = (4th plus 5th columns of detail.log) + (time difference between training done in server.log and finished training in client.log)
# as for lstm, due to failed client/server log, the second part of the runtime uses the value of resnet18 as an approximation.

# VeriFL
# raw logs at *Alpha/log_pets25/verifl/*
# parse logs
python3 Alpha/log_pets25/parse_verifl.py
# aggregated output *Alpha/log_pets25/verifl.csv*

# e-SeaFL
# raw logs at *Alpha/log_pets25/eseafl/*
# parse logs
python3 Alpha/log_pets25/parse_eseafl.py
# aggregated output *Alpha/log_pets25/eseafl.csv*
```

Section **Table 1** of the python notebook [*Alpha/log_pets25/pets.ipynb*](Alpha/log_pets25/pets.ipynb) includes the code to extract stats of AlphaFL.
**Table 2** reuses the numbers for AlphaFL from **Table 1, 3, 4**.
Results of RoFL, VeriFL, E-SeaFL are added manually from the logs or the csv files.

#### Main Result 3:
The new input commitment protocol and silent select protocol outperform existing solutions, as shown in **Section 7.6**.

Numbers in **Table 5** and **Table 6** are added manually from the raw logs in [Alpha/log_pets25/micro_input/](Alpha/log_pets25/micro_input/) and [Alpha/log_pets25/micro_select/](Alpha/log_pets25/micro_select/), as well as the *alpha_input_\*.csv* files.


### Experiments 

#### Experiment 1: AlphaFL Input Commitment
This experiment evaluates the runtime and communication overhead of AlphaFL's input commitment protocols.
Expected results are the log files at [Alpha/log_pets25/alpha_input/](Alpha/log_pets25/alpha_input/).

Relevant source files:
- [Alpha/alpha-client.cpp](Alpha/alpha-client.cpp) : Client side of the Input Commitment protocol
- [Alpha/alpha-server.cpp](Alpha/alpha-server.cpp) : Server side of the Input Commitment protocol
- [Alpha/alpha-pre.cpp](Alpha/alpha-pre.cpp) : Helpers of the Input Commitment protocol

```bash
# Usage: bash Alpha/benchmark.sh -b alpha-input -c [number of clients] -i [number of inputs] -m [mode: passive(Ho) or active(DiHo)] -r [number of repetitions]

# example
bash Alpha/benchmark.sh -b alpha-input -c 2 -i 2000 -m passive -r 1

# run all testcases in the paper, which might take long
bash Alpha/run.sh
```

#### Experiment 2: AlphaFL Secure Aggregation
This experiment evaluates the runtime and communication overhead of AlphaFL's secure aggregation protocol.
Expected results are the log files at [Alpha/log_pets25/alpha_sa/](Alpha/log_pets25/alpha_sa/).

Relevant source file:
- [Programs/Source/alpha_sa.mpc](Programs/Source/alpha_sa.mpc) : Source code of the Secure Aggregation protocol

```bash
# Usage: bash Alpha/benchmark.sh -b alpha-sa -c [number of clients] -i [number of inputs] -r [number of repetitions]

# example
bash Alpha/benchmark.sh -b alpha-sa -c 2 -i 2000 -r 1

# full execution is already included in *Alpha/run.sh*
```

#### Experiment 3: Microbenchmark - Input Commitment
This experiment intends to compare the input commitment protocol of AlphaFL with MP-SPDZ.
The AlphaFL's parts are already done in **Experiment 1**.
Thus we only benchmark MP-SPDZ here.
Expected results are commandline outputs like [Alpha/log_pets25/micro_input/](Alpha/log_pets25/micro_input/).

Relevant source files:
- [Alpha/alpha_input_client.cpp](Alpha/alpha_input_client.cpp) : Client side of the MP-SPDZ Input Commitment protocol
- [Programs/Source/alpha_input_orig.mpc](Programs/Source/alpha_input_orig.mpc) : Server side of the MP-SPDZ Input Commitment protocol

```bash
# Usage: bash Alpha/benchmark.sh -b micro-input -i [number of inputs]

# example
bash Alpha/benchmark.sh -b micro-input -i 2000

# full execution. WARNING: this compilation might crash the system if RAM is low!
for INP in 62006 274442 817872; do
    bash Alpha/benchmark.sh -b micro-input -i $INP
done
```

#### Experiment 4: Microbenchmark - Silent Select
This experiment intends to compare the select protocol of AlphaFL with MP-SPDZ.
Expected results are commandline outputs like [Alpha/log_pets25/micro_select/](Alpha/log_pets25/micro_select/).

Relevant source files:
- [Programs/Source/alpha_select_orig.mpc](Programs/Source/alpha_select_orig.mpc) : baseline - MP-SPDZ
- [Programs/Source/alpha_select_silent.mpc](Programs/Source/alpha_select_silent.mpc) : ours - AlphaFL

```bash
# Usage: bash Alpha/benchmark.sh -b micro-select -i [number of inputs]

# example
bash Alpha/benchmark.sh -b micro-select -i 2000

# full execution
for INP in 62006 274442 817872; do
    bash Alpha/benchmark.sh -b micro-select -i $INP
done
```

#### Experiment 5: End-to-End Baselines (Optional and Challenging)
The configurations and scripts are included in following folders:
- [Alpha/ELSA/](Alpha/ELSA) : for both Elsa and Prio+
- [Alpha/VeriFL](Alpha/VeriFL)
- [Alpha/e-SeaFL](Alpha/e-SeaFL)
- [Alpha/RoFL/](Alpha/RoFL) : most challenging, poorly documented and compatible

## Limitations (Only for Functional and Reproduced badges)
The main limitation is that even though we provide all the source code of our proof-of-concept implementation, the reviewer may not be able to re-run all the (large) testcases included in the paper, due to hardware (CPU and RAM) and time constraints.
We hope the reviewer could at least test on some small inputs.

The other limitation is with the baseline studies, especially RoFL.
The project is complex, yet poorly documented, which makes it difficult to evaluate.
Its compatibility is also a big issue.
Considering that we include the one-server setting only for completeness and we've already spent too much effort debugging this outdated baseline project, we decide to leave it like this.

## Notes on Reusability (Only for Functional and Reproduced badges)
Our work could be possibly integrated into MP-SPDZ, to benefit the whole security/crypto community.
1. The Square Correlations are implemented but disabled for rings in MP-SPDZ, which could be fixed.
2. The Silent Select protocol could be easily integrated into MP-SPDZ.
3. The Input Commitment involves some low-level functions such as vOLE, which might need more work.