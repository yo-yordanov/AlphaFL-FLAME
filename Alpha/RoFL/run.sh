# Large
nohup ./target/debug/config_server -c shakespeare_lstm.yml > server.log 2>&1 &

PYTHONPATH=$(pwd) nohup python trainservice/service.py --config ../shakespeare_lstm.yml > proxy.log 2>&1 &

nohup ./target/debug/flclients -n 4 -r 50016 > client.log 2>&1 &


# Small
nohup ./target/debug/config_server -c cifar10_lenet5.yml > server.log 2>&1 &

PYTHONPATH=$(pwd) nohup python trainservice/service.py --config ../cifar10_lenet5.yml > proxy.log 2>&1 &

nohup ./target/debug/flclients -n 4 -r 50016 > client.log 2>&1 &


# Medium
nohup ./target/debug/config_server -c cifar10_resnet18.yml > server.log 2>&1 &

PYTHONPATH=$(pwd) nohup python trainservice/service.py --config ../cifar10_resnet18.yml > proxy.log 2>&1 &

nohup ./target/debug/flclients -n 4 -r 50016 > client.log 2>&1 &