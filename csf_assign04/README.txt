CONTRIBUTIONS

Alan and Lauren worked together to write the assignment together as a whole. 

REPORT
Experiments and Analysis: 
Analysis: 
From the data below, we can see that a lower threshold produces a lower runtime which we can conclude that 
creating more parallel processes can decrease the runtime to some degree. As the number of parallel processes 
increase infinitely, the runtime will stop decreasing relatively and increase. This is because of how inherently 
merge sort works as it has O(n*logn) time complexity.

Data:
Test run with threshold 2097152

real    0m0.998s
user    0m0.950s
sys     0m0.036s
Test run with threshold 1048576

real    0m0.641s
user    0m0.947s
sys     0m0.047s
Test run with threshold 524288

real    0m0.765s
user    0m1.369s
sys     0m0.131s
Test run with threshold 262144

real    0m1.051s
user    0m2.285s
sys     0m0.259s
Test run with threshold 131072

real    0m1.633s
user    0m4.004s
sys     0m0.618s
Test run with threshold 65536

real    0m2.470s
user    0m7.180s
sys     0m1.376s
Test run with threshold 32768

real    0m3.522s
user    0m13.035s
sys     0m3.399s
Test run with threshold 16384

real    0m5.430s
user    0m25.958s
sys     0m9.379s