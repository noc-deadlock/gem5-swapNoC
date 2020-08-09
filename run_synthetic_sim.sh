#!/bin/bash

bench_caps=( 'UNIFORM_RANDOM' 'BIT_COMPLEMENT' 'SHUFFLE' )
bench=( 'uniform_random' 'bit_complement' 'shuffle' )
num_cores=64

out_dir="/nethome/tkrishna3/research/tools/gem5/projects/garnet2.0_dev/validation_stats"
date="2016_09_25"
cycles=50000
vnet=0
vc=16
tr=1

for b in 0 1 2
do
for i in 0.02 0.04 0.06 0.08 0.10 0.12 0.14 0.16 0.18 0.20 0.22 0.24 0.26 0.28 0.30 0.32 0.34 0.36 0.38 0.40 0.42 0.44 0.46
do
    ./build/Garnet_standalone/gem5.debug -d $out_dir/${num_cores}c/${bench_caps[$b]}/$date/tr-${tr}/vnet-${vnet}_vc-${vc}_i-${i} configs/example/garnet_synth_traffic.py --topology=Mesh_XY --num-cpus=${num_cores} --num-dirs=${num_cores} --mesh-rows=8 --network=garnet2.0 --router-latency=$tr --sim-cycles=$cycles --inj-vnet=${vnet} --vcs-per-vnet=16 -i ${i} --synthetic=${bench[$b]}
done
done
