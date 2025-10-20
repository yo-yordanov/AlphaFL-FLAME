#!/usr/bin/env bash

function microbenchmark_square {
    ./compile.py -R 64 alpha_square_mpspdz >/dev/null
    ./compile.py -R 64 alpha_square_mul >/dev/null
    ./compile.py -R 64 alpha_square_triple >/dev/null

    result="MP-SPDZ"$'\n'
    txt="$(Scripts/spdz2k.sh alpha_square_mpspdz --batch-size 1 | grep -A1 '^Time')"
    result="$result""$txt"$'\n'"MUL"$'\n'
    txt="$(Scripts/spdz2k.sh alpha_square_mul --batch-size 1 | grep -A1 '^Time')"
    result="$result""$txt"$'\n'"TRIPLE"$'\n'
    txt=$(Scripts/spdz2k.sh alpha_square_triple --batch-size 1 | grep -A1 '^Time')
    result="$result""$txt"
    echo "$result"
}

function microbenchmark_select {
    NUM=10
    if [ $# -ge 1 ]; then
        NUM=$1
    fi
    MODE="active"
    ./compile.py -Y -O -I -l -R 64 alpha_select_orig.mpc $NUM $MODE
    ./compile.py -Y -O -I -l -R 64 alpha_select_silent.mpc $NUM $MODE

    result="MP-SPDZ"$'\n'
    txt="$(Scripts/spdz2k.sh alpha_select_orig-$NUM-$MODE -F | grep -A1 '^Time')"
    result="$result""$txt"$'\n'"SILENT"$'\n'
    txt=$(Scripts/spdz2k.sh alpha_select_silent-$NUM-$MODE -F | grep -A1 '^Time')
    result="$result""$txt"
    echo "$result"
}

function microbenchmark_input {
    NUM_CLIENTS=1
    NUM_INPUTS=1
    if [ $# -ge 1 ]; then
        NUM_INPUTS=$1
    fi
    NUM_BITS=32
    MODE="active"

    ./compile.py -Y -O -I -l -R 64 alpha_input_orig.mpc $NUM_CLIENTS $NUM_INPUTS $NUM_BITS $MODE
    make -j alpha
    ./alpha_input_client.x 0 2 $NUM_INPUTS $NUM_BITS 1 &

    result="MP-SPDZ"$'\n'
    txt="$(Scripts/spdz2k.sh alpha_input_orig-$NUM_CLIENTS-$NUM_INPUTS-$NUM_BITS-$MODE -F | grep -A1 '^Time')"
    result="$result""$txt"
    echo "$result"
}

function benchmark_alpha_sa {
    NUM_CLIENTS=$1
    NUM_INPUTS=$2
    NUM_REP=$3
    LOG_DIR="$LOG"/alpha_sa/
    mkdir -p $LOG_DIR

    ./compile.py -Y -O -I -l -R 64 alpha_sa.mpc $NUM_CLIENTS $NUM_INPUTS

    # clear logs
    >"$LOG_DIR"/sa_"$NUM_CLIENTS"_"$NUM_INPUTS".log

    # benchmark
    for r in $(seq 1 $NUM_REP); do
        pkill -f "spdz2k-party.x"
        Scripts/spdz2k.sh alpha_sa-$NUM_CLIENTS-$NUM_INPUTS -F >>"$LOG_DIR"/sa_"$NUM_CLIENTS"_"$NUM_INPUTS".log 2>&1
    done
}

function benchmark_alpha_input {
    NUM_CLIENTS=$1
    NUM_INPUTS=$2
    NUM_REP=$4
    MODE=0
    if [ "$3" = "active" ]; then
        MODE=1
    fi
    LOG_DIR="$LOG"/alpha_input/
    mkdir -p $LOG_DIR

    make -j alpha

    # clear logs
    for i in $(seq 0 $((NUM_CLIENTS - 1))); do
        >"$LOG_DIR"/client_"$NUM_CLIENTS"_"$NUM_INPUTS"_"$MODE"_"$i".log
    done
    for i in 0 1; do
        >"$LOG_DIR"/server_"$NUM_CLIENTS"_"$NUM_INPUTS"_"$MODE"_"$i".log
    done

    # benchmark
    for r in $(seq 1 $NUM_REP); do
        pkill -f "alpha-*.x"
        for i in $(seq 0 $((NUM_CLIENTS - 1))); do
            ./alpha-client.x $i $NUM_CLIENTS $NUM_INPUTS $MODE >>"$LOG_DIR"/client_"$NUM_CLIENTS"_"$NUM_INPUTS"_"$MODE"_"$i".log 2>&1 &
        done
        ./alpha-server.x 1 $NUM_CLIENTS $NUM_INPUTS $MODE >>"$LOG_DIR"/server_"$NUM_CLIENTS"_"$NUM_INPUTS"_"$MODE"_1.log 2>&1 &
        ./alpha-server.x 0 $NUM_CLIENTS $NUM_INPUTS $MODE >>"$LOG_DIR"/server_"$NUM_CLIENTS"_"$NUM_INPUTS"_"$MODE"_0.log 2>&1
    done
}

function benchmark_flame_private {
    NUM_CLIENTS=$1
    NUM_INPUTS=$2
    NUM_REP=$3
    LOG_DIR="$LOG"/flame_private/
    mkdir -p $LOG_DIR

    ./compile.py -Y -O -I -l -R 64 flame_private.mpc $NUM_CLIENTS $NUM_INPUTS

    # clear logs
    >"$LOG_DIR"/fp_"$NUM_CLIENTS"_"$NUM_INPUTS".log

    # benchmark
    for r in $(seq 1 $NUM_REP); do
        pkill -f "spdz2k-party.x"
        Scripts/spdz2k.sh flame_private-$NUM_CLIENTS-$NUM_INPUTS -F >>"$LOG_DIR"/fp_"$NUM_CLIENTS"_"$NUM_INPUTS".log 2>&1
    done
}

function benchmark_flame_leaky {
    NUM_CLIENTS=$1
    NUM_INPUTS=$2
    NUM_REP=$3
    LOG_DIR="$LOG"/flame_leaky/
    mkdir -p $LOG_DIR

    ./compile.py -Y -O -I -l -R 64 flame_leaky.mpc $NUM_CLIENTS $NUM_INPUTS

    # clear logs
    >"$LOG_DIR"/fl_"$NUM_CLIENTS"_"$NUM_INPUTS".log

    # benchmark
    for r in $(seq 1 $NUM_REP); do
        pkill -f "spdz2k-party.x"
        Scripts/spdz2k.sh flame_leaky-$NUM_CLIENTS-$NUM_INPUTS -F >>"$LOG_DIR"/fl_"$NUM_CLIENTS"_"$NUM_INPUTS".log 2>&1
    done
}

function Help {
    echo "$0 -b alpha-input [-c #clients] [-i #inputs] [-m mode] [-r #repetitions]"
}

LOG="Alpha/log_pets25/"
target=""
clients=1
inputs=1
mode="passive"
repetitions=5

while getopts ":h:b:c:i:m:r:" option; do
    case $option in
    h) # display Help
        Help
        exit
        ;;
    b) # benchmarking
        target=$OPTARG
        ;;
    c) # number of clients
        clients=$OPTARG
        ;;
    i) # number of inputs
        inputs=$OPTARG
        ;;
    m) # mode
        mode=$OPTARG
        ;;
    r) # repetition
        repetitions=$OPTARG
        ;;
    \?) # Invalid option
        echo "Error: Invalid option"
        exit
        ;;
    esac
done

case $target in
"alpha-input")
    benchmark_alpha_input $clients $inputs $mode $repetitions
    exit
    ;;
"alpha-sa")
    benchmark_alpha_sa $clients $inputs $repetitions
    exit
    ;;
"micro-select")
    microbenchmark_select $inputs
    exit
    ;;
"micro-input")
    microbenchmark_input $inputs
    exit
    ;;
"flame-private")
    benchmark_flame_private $clients $inputs $repetitions
    exit
    ;;
"flame-leaky")
    benchmark_flame_leaky $clients $inputs $repetitions
    exit
    ;;
*) # Invalid option
    echo "Error: Invalid target"
    exit
    ;;
esac
