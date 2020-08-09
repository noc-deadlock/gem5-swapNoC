#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM')
bench=( 'uniform_random')


buffer_depth=( '2' '4' '6' '8' '10' '12' '14' )
num_cores=64

# out_dir="/usr/scratch/mayank/swap/validation_stats/wormhole/tornado"
# bench_caps=$1
# bench=$2
out_dir="/usr/scratch/mayank/intraswap_rslt/wormhole_baseline"
date="2017_1_27"
cycles=10000
vnet=0
vc=1
tr=1 

# for b in 0 1 2 3 4 5 6 7 8 9 10
# do
for i in 0.02 0.04 0.06 0.08 0.10 0.12 0.14 0.16 0.18 0.20 0.22 0.24 0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46
do
for depth in 0 1 2 3 4 5 6
do
    ./build/Garnet_standalone/gem5.debug -d $out_dir/${num_cores}c/$date/${bench_caps}/tr-${tr}/vnet-${vnet}_vc-${vc}_depth-${buffer_depth[$depth]}/i-${i} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=${tr} --sim-cycles=${cycles} --enable-wormhole=1 --wormhole-buffer-depth=${buffer_depth[$depth]} --inj-vnet=${vnet} --injectionrate=${i} --synthetic=${bench} &
done
done
# done

