#!/bin/bash
######
# Taken from https://github.com/emp-toolkit/emp-readme/blob/master/scripts/throttle.sh
######

## replace DEV=lo with your card (e.g., eth0)
DEV=lo 
if [ "$1" == "del" ]
then
	sudo tc qdisc del dev $DEV root
fi

if [ "$1" == "lan-fq" ]
then
sudo tc qdisc del dev $DEV root
## 2.5G per connection, to avoid crash
sudo tc qdisc add dev $DEV root fq maxrate 5gbit
fi
if [ "$1" == "lan-tbf" ]
then
sudo tc qdisc del dev $DEV root
## about 10Gbps
sudo tc qdisc add dev $DEV root handle 1: tbf rate 10gbit burst 10mb latency 1ms
## about 1ms ping latency
sudo tc qdisc add dev $DEV parent 1:1 handle 10: netem delay 0.47ms
fi
if [ "$1" == "wan" ]
then
sudo tc qdisc del dev $DEV root
## about 400Mbps
sudo tc qdisc add dev $DEV root handle 1: tbf rate 400mbit burst 100000 limit 10000
## about 40ms ping latency
sudo tc qdisc add dev $DEV parent 1:1 handle 10: netem delay 20msec
fi
