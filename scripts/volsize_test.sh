#!/bin/bash

#size=$((512 / (2**($2-1))))
size=$((512 / $2))

#resample 
~/afront/src/afront/hafront.sh -nogui $1.nhdr -save_ooc ~/data/volumes/size_test $1_r$size 10000 10000 10000  0 0 0  0 0 0  $2

# run afront
(time ~/afront/src/afront/hafront.sh -nogui -noedgeflip ~/data/volumes/size_test/$1_r$size.blocks/$1_r$size.000.000.000.nhdr -rho 0.5 -outname ~/data/volumes/size_test/$1_r$size.m -tri 45.1234 bspline 2>~/data/volumes/size_test/$1_r$size.out.txt) 2>~/data/volumes/size_test/$1_r$size.time.txt & afront_pid=$!


# wait and collect mem info
rm -f ~/data/volumes/size_test/$1_r$size.mem.txt
while ps $afront_pid >/dev/null; do
    top -b -n1 | grep 'afront ' >> ~/data/volumes/size_test/$1_r$size.mem.txt
    sleep 3
done
