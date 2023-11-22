CONTRIBUTIONS

Alan and Lauren worked together to write the assignment together as a whole. Lauren worked on fleshing out the main logic and generating the report, while Alan worked on the writeup and error handling.

REPORT

Experiment Overview:
We generated a random file with 16 MB of random data, then tried sorting this file at multiple thresholds. As the thresholds decreased,
the parallelism increased. From the data below, we can see an overall trend that a lower threshold produces a lower runtime which helps
us conclude that creating more parallel processes can speed up the runtime to some degree. However, after reaching a certain level of 
parallelism, we can see a point of diminishing returns, which is dependent on the CPU cores available. By running `grep -c processor /proc/cpuinfo`, 
we can see that there are 8 cores on the server, and the point of diminishing returns should likely be when full core usage occurs, so there
is less benefit from increasing the number of parallel processes further.

Data:
Test run with threshold 2097152

real    0m0.384s
user    0m0.373s
sys     0m0.009s
Test run with threshold 1048576

real    0m0.232s
user    0m0.393s
sys     0m0.028s
Test run with threshold 524288

real    0m0.181s
user    0m0.461s
sys     0m0.033s
Test run with threshold 262144

real    0m0.158s
user    0m0.528s
sys     0m0.040s
Test run with threshold 131072

real    0m0.145s
user    0m0.537s
sys     0m0.066s
Test run with threshold 65536

real    0m0.158s
user    0m0.559s
sys     0m0.090s
Test run with threshold 32768

real    0m0.161s
user    0m0.575s
sys     0m0.121s
Test run with threshold 16384

real    0m0.173s
user    0m0.596s
sys     0m0.165s

Analysis:
The real time indicates the amount of time that elapsed between when the program started and exited, so we will use it as a measure
of how long sorting took. From this, we can see that when running at a threshold of 131072, there was the lowest time, with 0.145s.
Furthermore, we see that longest time was from the highest threshold of 2097152, where it took 0.384s, thus, by increasing the number
of parallel processes 16-fold, there was a roughly 1.648 times speedup in the merge sort. This is likely because the OS kernel can 
schedule these processes on the CPU to run in parallel (at the same time), which will allow merge sort to speed up massively as each
individual half of the divided list is being sorted recursively at the same time. We also note that the speedup from 262144 (8 processes) 
to 131072 (16 processe) is minimal, around 0.0897 times faster. This could be because we have reached the max parallel processes being run 
on separate CPU cores, where there are 8 cores, so increasing past this would mean scheduling on the same core again. Indeed, after a certain 
point (below 131072), there is even a slowing down, with increasing sorting times. This could potentially be because we created too many
child processes which have to be individually managed and switch contexts, and are not being scheduled on new cores, so they are slowing down 
the processing time in general. Creating too many parallel processes can potentially be detrimental to speed. We note that mergesort has O(n log n)
time complexity, so it is mainly beneficial with very large lists, and the speedup when you make the list smaller is more negligible. This
also helps explain the proportionally diminishing returns when the number of processes are increased and the list is split into increasingly
smaller sizes. 