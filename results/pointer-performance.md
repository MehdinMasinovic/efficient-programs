# Pointer Performance Analysis

## Overview
Analysis of the "pointer" implementation's performance metrics to evaluate efficiency, cache performance, and CPU utilization.

## Idea
Building on top of the general-improvements implementation, the "pointer" implementation aims to enhance the performance of the program by introducing pointers. Until now, for each element, the string value was stored in the `struct` itself. Now, a pointer to the beginning of the string and the length of it are used to access the value of the key/value.

# Failed Ideas:
To further improve the performance, a string-table was additionally stored where for each string one representative pointer and length were stored. The idea behind this was that each time one string is either the key or value of a record, the record will store the address of the representative string, so that strings no longer have to be stored, but instead just the addresses. However, this variation took significantly longer. Probably, because the string table (map) was too big to fit in the cache and resulted in many cache misses.

## Basic CPU Metrics

```bash
LC_NUMERIC=en_US perf stat -e cycles,instructions,branches,branch-misses pointer
```

```
                   Pointer
cycles:            131.397 
instructions:      143.249 
IPC:               1.09
branches:          31.70B
branch-misses:     0.24B (0.78%)
Time elapsed:      34.43s
User time:         33.47s
System time:       0.96s
```

### Analysis
- Sub 1 second system time tells us that the program is not spending much time in the kernel.
- Branch misprediction rate of 0.78% is very low, showing highly efficient branch prediction.

## Cache Performance

```
                    Reference 
L1-dcache-misses:   619.95M (1.24%)
L1-dcache-loads:    49.85B
LLC-load-misses:    232.75M 
```

### Analysis
- Cache miss rate almost stayed the same.
- The total number of L1 cache misses dropped from 689.7M to 619.95M, indicating that fewer data cache load operations failed in the first level, contributing to faster data access.
- The LLC (Last-Level Cache) misses reduced marginally from 245.67M to 232.75M, showing slightly better handling of data beyond the L1 cache.


## User vs Kernel Mode Analysis

```
                        Pointer
cycles (kernel):        4.94B 
cycles (user):          126.46B 
instructions (kernel):  5.86B
instructions (user):    137.39B
IPC (kernel):           1.19
IPC (user):             1.09
```

### Analysis
- Significant decrease of cycles in kernel mode
- User mode IPC lower than kernel mode IPC
- IPC performance in general decreased


## Key Optimization Opportunities
1. **Instruction Efficiency**
   - IPC of 1.09 suggests some stalls in the instruction pipeline. Compared to other implementations, this is lower. Maybe some algorithmic optimization can help.

2. **String view**
   - C++ 17 provides a `std::string_view` class that can be used to store a pointer to a string and its length. The class also supports other functions (hashing). Utilizing this class could potentially improve performance. Because right now for hashing, the address needs to be casted to a string.

3. **Runtime Performance**
   - Compared to the reference implementation, the runtime is still significantly higher. Parallelization could be a way to improve this.

## Pointer Performance Summary
- Total Cycles: ~131.4 billion
- Total Instructions: ~143.2 billion
- Branches: ~31 billion
- Branch Misses: ~242 million (~0.78%)
- IPC: 1.09

While the "pointer" implementation demonstrates efficient branch prediction and overall less cycles, the IPC performance decreased compared to other implementations. 
