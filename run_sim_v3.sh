#!/bin/bash
################
./run_tornado_random_wormhole_intraswap.sh BIT_REVERSE bit_reverse
echo "./run_tornado_random_wormhole_intraswap.sh BIT_REVERSE bit_reverse"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole_intraswap.sh BIT_ROTATION bit_rotation
echo "./run_tornado_random_wormhole_intraswap.sh BIT_ROTATION bit_rotation"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole_intraswap.sh NEIGHBOR neighbor
echo "./run_tornado_random_wormhole_intraswap.sh NEIGHBOR neighbor"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole_intraswap.sh TRANSPOSE transpose
echo "./run_tornado_random_wormhole_intraswap.sh TRANSPOSE transpose"
sleep 1200 #change it to 1800sec
###############
./run_tornado_random_wormhole.sh BIT_REVERSE bit_reverse
echo "./run_tornado_random_wormhole.sh BIT_REVERSE bit_reverse"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole.sh BIT_ROTATION bit_rotation
echo "./run_tornado_random_wormhole.sh BIT_ROTATION bit_rotation"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole.sh NEIGHBOR neighbor
echo "./run_tornado_random_wormhole.sh NEIGHBOR neighbor"
sleep 1200 #change it to 1800sec

./run_tornado_random_wormhole.sh TRANSPOSE transpose
echo "./run_tornado_random_wormhole.sh TRANSPOSE transpose"
sleep 1200 #change it to 1800sec

###############