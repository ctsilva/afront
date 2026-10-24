#!/usr/bin/python

import sys
import subprocess
import time


hosts = (("sun", 4),
         ("darkphoenix", 4),
#         ("juggernaut", 2),
#         ("hex", 8),
#         ("octagon", 4),
#         ("arachne", 8),
         ("vis-2", 2),
         ("vis-3", 2),
         ("vis-4", 2),
         ("vis-5", 2),
         ("vis-6", 2),
         ("vis-7", 2))


if "echo" in sys.argv:
    echo = "echo "
else:
    echo = ""


# make our jobs nice
nice = " nice "


nooc = sys.argv[1]
params = sys.argv[-1]

for line in open(nooc):

    if line.startswith("block size: "):
        tokens = line.strip().split(" ")
        blocksizes = (int(tokens[-3]), int(tokens[-2]), int(tokens[-1]))

    if line.startswith("sizes: "):
        tokens = line.strip().split(" ")
        sizes = (int(tokens[-3]), int(tokens[-2]), int(tokens[-1]))

    if line.startswith("block prefix: "):
        tokens = line.strip().split(" ")
        block_prefix = tokens[-1]


lblock_prefix = "/".join(nooc.split("/")[0:-1]) + "/" + block_prefix

nblocks = [0,0,0]
for i in xrange(3):
    nblocks[i] = sizes[i] / blocksizes[i]
    if (sizes[i] % blocksizes[i]):
        nblocks[i] += 1
 
numblocks = nblocks[0]*nblocks[1]*nblocks[2]


#
# initial setup - copying data and setting up binary
#
if "nosetup" not in sys.argv:
    for i in xrange(len(hosts)):
        # make dirs
        subprocess.Popen(echo + "ssh " + hosts[i][0] + " rm -rf /tmp/afront_dist ", shell=True).wait()
        subprocess.Popen(echo + "ssh " + hosts[i][0] + " mkdir /tmp/afront_dist ", shell=True).wait()
        subprocess.Popen(echo + "ssh " + hosts[i][0] + " mkdir /tmp/afront_dist/data ", shell=True).wait()
        subprocess.Popen(echo + "ssh " + hosts[i][0] + " mkdirhier /tmp/afront_dist/data/" + ("/".join(block_prefix.split("/")[0:-1])), shell=True).wait()
        subprocess.Popen(echo + "ssh " + hosts[i][0] + " mkdir /tmp/afront_dist/oocgf ", shell=True).wait()
        
        # copy binary
        subprocess.Popen(echo + "scp /tmp/afront " + hosts[i][0] + ":/tmp/afront_dist/ ", shell=True).wait()

        # copy .nooc
        subprocess.Popen(echo + "scp " + nooc + " " + hosts[i][0] + ":/tmp/afront_dist/data/ ", shell=True).wait()

        # copy block data
        subprocess.Popen(echo + "scp " + ("/".join(lblock_prefix.split("/")[0:-1])) + "/* " + hosts[i][0] + ":/tmp/afront_dist/data/" + ("/".join(block_prefix.split("/")[0:-1])) + "/", shell=True).wait()



#
# clear out the gf dir
#
for i in xrange(len(hosts)):
    # make dirs
    subprocess.Popen(echo + "ssh " + hosts[i][0] + " rm /tmp/afront_dist/oocgf/\* ", shell=True).wait()



#
# run one gf block on each host until they're all done
#
nextblock = 0
doneblocks = 0
processes = [None for x in hosts]
while True:
    
    # find next spot to run one
    for i in xrange(len(processes)):
        if processes[i] is None:
            # no processes in this slot
            if nextblock < numblocks:
                processes[i] = subprocess.Popen(echo + "ssh "  + hosts[i][0] + " " +
                                                "\"" +
                                                "cd /tmp/afront_dist " + 
                                                " && " + 
                                                nice + "./afront -nogui ./data/" + (nooc.split("/")[-1]) + " " +
                                                "-idealNumThreads " + str(hosts[i][1]) + " " +
                                                params.replace("-tri ", "-make_gf " + str(nextblock) + " ") + " " +
                                                "\"" +
                                                "",
                                                shell=True)

                nextblock += 1


        elif processes[i].poll() is not None:
            # done running
            doneblocks += 1
            processes[i] = None


    if (doneblocks == numblocks):
        break

    time.sleep(1)


#
# copy the gf dir back here
#
for i in xrange(len(hosts)):
    # make dirs
    subprocess.Popen(echo + "scp " + hosts[i][0] + ":/tmp/afront_dist/oocgf/* ./oocgf", shell=True).wait()
