REP=4

# ELSA
LOG="logs/elsa/"
mkdir -p $LOG
for G in 100000 300000; do
    for N in 10 20 30 40; do
        for r in $(seq 1 $REP); do
            #Alice
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g $G -n $N -m 7777 -p 6666 -s 16 -i 32 >>$LOG/alice_"$N"_"$G".log 2>&1 &
            #Bob
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g $G -n $N -m localhost:7777 -b -p 6667 -s 16 -i 32 >>$LOG/bob_"$N"_"$G".log 2>&1 &
            #Clients
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-mp -- -g $G -n $N -a localhost:6666 -b localhost:6667 -i 32 >>$LOG/client_"$N"_"$G".log 2>&1
            sleep 60s
        done
    done
done

# Prio
LOG="logs/prio/"
mkdir -p $LOG
for G in 100000 300000; do
    for N in 10 20 30 40; do
        for r in $(seq 1 $REP); do
            #Clients
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-baseline-mp -- -g $G -n $N -a localhost:6666 -b localhost:6667 -i 32 >>$LOG/client_"$N"_"$G".log 2>&1 &
            #Alice
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-baseline-mp -- -g $G -n $N -m 7777 -p 6666 -s 16 -i 32 >>$LOG/alice_"$N"_"$G".log 2>&1 &
            #Bob
            RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-baseline-mp -- -g $G -n $N -m localhost:7777 -b -p 6667 -s 16 -i 32 >>$LOG/bob_"$N"_"$G".log 2>&1
            sleep 60s
        done
    done
done

exit

# set prio version in baseline-mp
cargo update -p home@0.5.11 --precise 0.5.5

#Alice
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 817872 -n 1 -m 7777 -p 6666 -s 16 -i 32 >lstm_alice_1.log 2>&1
#Bob
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 817872 -n 1 -m localhost:7777 -b -p 6667 -s 16 -i 32 >lstm_bob_1.log 2>&1
#Clients
RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-mp -- -g 817872 -n 1 -a localhost:6666 -b localhost:6667 -i 32 >lstm_client_1.log 2>&1

#Alice
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 274442 -n 1 -m 7777 -p 6666 -s 16 -i 32 >resnet18_alice_1.log 2>&1
#Bob
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 274442 -n 1 -m localhost:7777 -b -p 6667 -s 16 -i 32 >resnet18_bob_1.log 2>&1
#Clients
RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-mp -- -g 274442 -n 1 -a localhost:6666 -b localhost:6667 -i 32 >resnet18_client_1.log 2>&1

#Alice
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 62006 -n 1 -m 7777 -p 6666 -s 16 -i 32 >lenet5_alice_1.log 2>&1
#Bob
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-mp -- -g 62006 -n 1 -m localhost:7777 -b -p 6667 -s 16 -i 32 >lenet5_bob_1.log 2>&1
#Clients
RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-mp -- -g 62006 -n 1 -a localhost:6666 -b localhost:6667 -i 32 >lenet5_client_1.log 2>&1


RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-baseline-mp -- -g 100000 -n 10 -m 7777 -p 6666 -s 16 -i 32
RUSTFLAGS='-C target-cpu=native' cargo run --release --package server-baseline-mp -- -g 100000 -n 10 -m localhost:7777 -b -p 6667 -s 16 -i 32
RUSTFLAGS='-C target-cpu=native' cargo run --release --package client-baseline-mp -- -g 100000 -n 10 -a localhost:6666 -b localhost:6667 -i 32