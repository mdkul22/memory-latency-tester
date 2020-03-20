#!/bin/bash

rm latencyLog2.csv
make
./cachetime -t:6 -r --reps:10000 --log2array:16
./cachetime -t:6 -r -m --reps:10000 --log2array:16
