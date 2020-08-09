#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM' 'TORNADO_RANDOM_30' )
bench=( 'uniform_random' 'tornado_random_30' )

out_dir="/usr/scratch/mayank/nocs_intraswap_rslt/intel_swapnoc_sweep/singleflit_pkt"
cycles=10000
vnet=0 #for multi-flit pkt
num_cores=64
vc=1
tr=1
# depth=12 #fixed depth wormhole-queue
link_width_bits=64
################# Give attention to the injection rate that you have got#############################
for b in 0 1
do
for depth in 4 8 12 16
do
for (( i = 1; i < ${depth}; i++ )); 
do
for k in 0.02 0.04 0.06 0.08 0.10 0.12 0.14 0.16 0.18 0.20 0.22 0.24 0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46
do
    ./build/Garnet_standalone/gem5.debug -d $out_dir/${num_cores}c/${bench_caps[$b]}/tr-${tr}/vnet-${vnet}_vc-${vc}_depth-${depth}_inj-${k}/threshold-${i} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=$tr  --sim-cycles=$cycles --enable-wormhole=1 --enable-intraswap=1 --wormhole-buffer-depth=${depth} --intraswap-threshold=${i} --inj-vnet=${vnet} --injectionrate=${k} --synthetic=${bench[$b]} &
done
done
done
done
