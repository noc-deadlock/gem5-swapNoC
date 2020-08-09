#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM' 'TORNADO' 'TORNADO_RANDOM_30' 'TORNADO_RANDOM_70' 'BIT_COMPLEMENT' 'BIT_REVERSE' 'BIT_ROTATION' 'TRANSPOSE' 'SHUFFLE' 'NEIGHBOR' 'EDGE_50' )
bench=( 'uniform_random' 'tornado' 'tornado_random_30' 'tornado_random_70' 'bit_complement' 'bit_reverse' 'bit_rotation' 'transpose' 'shuffle' 'neighbor' 'edge_50' )

out_dir="/usr/scratch/mayank/nocs_intraswap_rslt/creditbased_swapnoc/singleflit_pkt"
cycles=10000
vnet=0 #for multi-flit pkt
num_cores=64
vc=1
tr=1
depth=4 #fixed depth wormhole-queue
link_width_bits=64
################# Give attention to the injection rate that you have got#############################
for b in 0 1 2 3 4 5 6 7 8 9 10
do
for k in 0.02 0.04 0.06 0.08 0.10 0.12 0.14 0.16 0.18 0.20 0.22 0.24 0.26 0.28: 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46 0.48 0.50
do
    ./build/Garnet_standalone/gem5.debug -d $out_dir/${num_cores}c/${depth}/${bench_caps[$b]}/tr-${tr}/vnet-${vnet}_vc-${vc}_depth-${depth}_inj-${k} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=$tr  --sim-cycles=$cycles --enable-wormhole=1 --enable-intraswap=1 --wormhole-buffer-depth=${depth} --intraswap-threshold=2 --inj-vnet=${vnet} --injectionrate=${k} --synthetic=${bench[$b]} &
done
done
