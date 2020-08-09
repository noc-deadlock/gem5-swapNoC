#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM' 'TORNADO' 'TORNADO_RANDOM_30' 'TORNADO_RANDOM_70' 'BIT_COMPLEMENT' 'BIT_REVERSE' 'BIT_ROTATION' 'TRANSPOSE' 'SHUFFLE' 'NEIGHBOR' 'EDGE_50' )
bench=( 'uniform_random' 'tornado' 'tornado_random_30' 'tornado_random_70' 'bit_complement' 'bit_reverse' 'bit_rotation' 'transpose' 'shuffle' 'neighbor' 'edge_50' )

out_dir="/usr/scratch/mayank/nocs_intraswap_rslt/intel_swapnoc/multiflit_pkt"
cycles=10000
vnet=2 #for multi-flit pkt
num_cores=64
vc=1
tr=1
depth=$1 #fixed depth wormhole-queue
link_width_bits=64
################# Give attention to the injection rate that you have got#############################
for b in 0 1 2 3 4 5 6 7 8 9 10
do
for (( i = 1; i < ${depth}; i++ )); 
do
for k in 0.002 0.004 0.006 0.008 0.010 0.012 0.014 0.016 0.018 0.020 0.022 0.024 0.026 0.028 0.030 0.032 0.034 0.036 0.038 0.040 0.042 0.044 0.046 0.048 0.050
do
    ./build/Garnet_intel_swapnoc/gem5.debug -d $out_dir/${num_cores}c/${depth}/${bench_caps[$b]}/tr-${tr}/vnet-${vnet}_vc-${vc}_depth-${depth}_inj-${k}/threshold-${i} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=$tr  --sim-cycles=$cycles --enable-wormhole=1 --enable-intraswap=1 --wormhole-buffer-depth=${depth} --intraswap-threshold=${i} --inj-vnet=${vnet} --injectionrate=${k} --synthetic=${bench[$b]} &
done
done
sleep 500
done
