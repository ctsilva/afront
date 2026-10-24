#!/bin/bash

#size=$((512 / (2**($2-1))))
size=$((512 / $2))

#resample 
~/afront/src/afront/hafront.sh -nogui $1.nhdr -save_ooc ~/data/volumes/ooc_test $1_$size $size $size $size  0 0 0  0 0 0  1

# run afront
(time ~/afront/src/afront/hafront.sh -nogui -noedgeflip ~/data/volumes/ooc_test/$1_$size.nooc -rho 0.5 -outname ~/data/volumes/ooc_test/$1_$size.m -tri 45.1234 bspline 2>~/data/volumes/ooc_test/$1_$size.out.m) 2>~/data/volumes/ooc_test/$1_$size.time.txt & afront_pid=$!

rm -f ~/data/volumes/ooc_test/$1_$size.mem.txt
while ps $afront_pid >/dev/null; do
    top -b -n1 | grep 'afront ' >> ~/data/volumes/ooc_test/$1_$size.mem.txt
    sleep 3
done



# run marching cubes
(time ~/afront/src/afront/hafront.sh -nogui -noedgeflip ~/data/volumes/ooc_test/$1_$size.nooc -rho 0.5 -outname ~/data/volumes/ooc_test/$1_$size.mc.m -marchingcubes 45.1234 bspline 2>~/data/volumes/ooc_test/$1_$size.out.mc.m) 2>~/data/volumes/ooc_test/$1_$size.time.mc.txt  & afront_pid=$!

rm -f ~/data/volumes/ooc_test/$1_$size.mem.mc.txt
while ps $afront_pid >/dev/null; do
    top -b -n1 | grep 'afront ' >> ~/data/volumes/ooc_test/$1_$size.mem.mc.txt
    sleep 3
done




# # run afront on the original dataset
# (time ~/afront/src/afront/hafront.sh -nogui -noedgeflip $1.nhdr -rho 0.5 -outname ~/data/volumes/ooc_test/$1.m -tri 45.1234 bspline 2>~/data/volumes/ooc_test/$1.out.m) 2>~/data/volumes/ooc_test/$1.time.txt & afront_pid=$!
# top -b | grep 'afront ' > ~/data/volumes/ooc_test/$1.mem.txt & mem_pid=$!
# wait $afront_pid
# kill $mem_pid

# # run mc on the original dataset
# (time ~/afront/src/afront/hafront.sh -nogui -noedgeflip $1.nhdr -rho 0.5 -outname ~/data/volumes/ooc_test/$1.mc.m -marchingcubes 45.1234 bspline 2>~/data/volumes/ooc_test/$1.out.mc.m) 2>~/data/volumes/ooc_test/$1.time.mc.txt  & afront_pid=$!
# top -b | grep 'afront ' > ~/data/volumes/ooc_test/$1.mem.mc.txt & mem_pid=$!
# wait $afront_pid
# kill $mem_pid
