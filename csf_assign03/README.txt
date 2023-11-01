TODO: add information about contributions of team member(s)
MS1: 
Lauren and Alan worked together on MS1 to draft and plan out the design of the cache simulator. 
Lauren worked on the Makefile while Alan worked on framework of main.cpp

MS2:
Lauren and Alan worked together on MS2 to design the LRU simulator components. Lauren worked on the main
function implementation while Alan focused on fleshing the logic of util.cpp.

MS3:
Alan worked on implemented FIFO while Lauren worked on testing the different cache types


Best Cache: 
Overall, the best performing cache is as follows: large size, either fully- or set-associative cache, write-allocate, write-back, and 
either lru or fifo
1. Varying types of caches:
    direct-mapped:		./csim 256 1 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 298863
    Load misses: 19334
    Store hits: 185202
    Store misses: 12284
    Total cycles: 20324767
	set-associative:	./csim 16 16 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 312951
    Load misses: 5246
    Store hits: 187632
    Store misses: 9854
    Total cycles: 10918737
	fully-associative:	./csim 1 256 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 313123
    Load misses: 5074
    Store hits: 187653
    Store misses: 9833
    Total cycles: 10812316

    Based on this data, we can see that there are the most load misses and the most number of cycles for direct-mapped caches.
    This indicates to us that direct-mapped caches perform the poorest, with set-associative and fully-associative caches performing
    similarly to each other but much better than direct-mapped caches. 

2. Varying size of caches: 
    small: ./csim 4 4 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 226203
    Load misses: 91994
    Store hits: 158644
    Store misses: 38842
    Total cycles: 77596925
    medium: ./csim 32 32 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314946
    Load misses: 3251
    Store hits: 188273
    Store misses: 9213
    Total cycles: 9277696
    large: ./csim 128 128 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 315855
    Load misses: 2342
    Store hits: 188617
    Store misses: 8869
    Total cycles: 5008952
    xlarge: ./csim 1024 1024 16 write-allocate write-back lru < traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 315855
    Load misses: 2342
    Store hits: 188617
    Store misses: 8869
    Total cycles: 5008952

    As the size of the cache increases, we notice that load misses, store misses, and total cycles decreases indicating that
    larger caches perform better than smaller caches. However, we can see that between a large cache and an extra large cache
    the data does not vary that much, which indicates that the statistics relative to cache size plateaus after a certain extent.

3. Varying combinations of parameters: 
    A: ./csim 256 4 16 write-allocate write-back lru < ./traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 9353719
    B: ./csim 256 4 16 write-allocate no-write-back lru < ./traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 25130033
    C: ./csim 256 4 16 no-write-allocate no-write-back lru < ./traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 311613
    Load misses: 6584
    Store hits: 164819
    Store misses: 32667
    Total cycles: 22700397

    Based on this data, the best performing paramters is write-allocate write-back. Compared to the similarly performing write-allocate
    no-write-back, it had less cycles during its performance. The worst performing parameters is no-write-allocate, no-write-back, as it 
    had the highest number of cycles, load misses, and store misses. 

4. Varying eviction types:
    LRU: ./csim 256 4 16 write-allocate write-back lru < ./traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 9353719
    FIFO: ./csim 256 4 16 write-allocate write-back fifo < ./traces/gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 9353719

    Based on this information, we can see that lru and fifo eviction types work equally well 

