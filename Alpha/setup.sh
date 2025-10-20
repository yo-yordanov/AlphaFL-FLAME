# Configure and compile MP-SPDZ for online-only benchmarking.
make -j9 Fake-Offline.x spdz2k-party.x

# Prepare SSL certs
Scripts/setup-ssl.sh 64
Scripts/setup-clients.sh 1

# Prepare offline data
./Fake-Offline.x 2 -Z 64 -S 64 -e 32,64

# Prepare persistent data for Silent-Select
./compile.py -Y -O -I -l -R 64 alpha_select_pre.mpc
Scripts/spdz2k.sh alpha_select_pre -F
mv Persistence/Transactions-P0.data data.tmp
mv Persistence/Transactions-P1.data Persistence/Transactions-P0.data
mv data.tmp Persistence/Transactions-P1.data
mv Player-Data/2-Z64,64-64/Player-MAC-Keys-R-P0 data.tmp
mv Player-Data/2-Z64,64-64/Player-MAC-Keys-R-P1 Player-Data/2-Z64,64-64/Player-MAC-Keys-R-P0
mv data.tmp Player-Data/2-Z64,64-64/Player-MAC-Keys-R-P1

# Repeat to fix possible issues with above setup
./Fake-Offline.x 2 -Z 64 -S 64 -e 1,14,15,16,30,31,32,33,47,63,64