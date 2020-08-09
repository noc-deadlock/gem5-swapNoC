#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM' 'TORNADO_RANDOM_30' )
bench=( 'uniform_random' 'tornado_random_30' )

out_dir="/usr/scratch/mayank/intraswap_rslt/swapnoc_sweep/multiflit_pkt"
cycles=10000
vnet=2 #for multi-flit pkt
num_cores=64
vc=1
tr=1
depth=12 #fixed depth wormhole-queue
link_width_bits=64
################# Give attention to the injection rate that you have got#############################
for b in 0 1
do
for depth in 4 8 12 16
do
for (( i = 1; i < ${depth}; i++ )); do
for k in 0.002 0.004 0.006 0.008 0.010 0.012 0.014 0.016 0.018 0.020 0.022 0.024 0.026 0.028 0.030 0.032 0.034 0.036 0.038 0.040 0.042 0.044 0.046
do
    ./build/Garnet_standalone/gem5.debug -d $out_dir/${num_cores}c/${depth}/${bench_caps[$b]}/tr-${tr}/vnet-${vnet}_vc-${vc}_depth-${depth}_inj-${k}/threshold-${i} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=$tr --link-width-bits=${link_width_bits} --sim-cycles=$cycles --enable-wormhole=1 --enable-intraswap=1 --wormhole-buffer-depth=${depth} --intraswap-threshold=${i} --inj-vnet=${vnet} --injectionrate=${k} --synthetic=${bench[$b]} &
done
done
sleep 300 #sleep for 5 mins
done
done
