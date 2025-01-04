# Baseline Hashmap Performance Analysis

## Overview
Analysis of the hashmap-based implementation's performance characteristics using `perf stat` with various metrics.

## Basic CPU Metrics

```
cycles:          237,984,408,590
instructions:    354,453,181,046  (1.49 insn per cycle)
branches:         73,652,325,881
branch-misses:       466,554,659  (0.63% of all branches)
Time elapsed:         59.95 seconds
User time:            56.58 seconds
System time:           3.37 seconds
```

### Analysis
- IPC (Instructions Per Cycle) of 1.49 matches reference implementation
- Branch misprediction rate of 0.63% is lower than reference (2.11%)
- ~94.4% of time spent in user mode, ~5.6% in system calls

## Cache Performance

```
L1-dcache-load-misses:   1,489,510,083  (1.32% of L1-dcache accesses)
L1-dcache-loads:       112,798,279,843
LLC-load-misses:           483,825,493
Time elapsed:               59.95 seconds
User time:                  56.58 seconds
System time:                 3.37 seconds
```

### Analysis
- L1 cache miss rate of 1.32% is better than both reference (3.55%) and baseline (2.64%)
- LLC misses (483M) are comparable to reference (351M)
- Significant improvement in cache utilization from baseline

## User vs Kernel Mode Analysis

```
cycles (kernel):     17,921,411,243
cycles (user):      220,167,758,796
instructions (kernel): 22,155,016,225  (1.24 insn per cycle)
instructions (user):  332,053,274,896  (1.51 insn per cycle)
Time elapsed:             59.95 seconds
User time:                56.58 seconds
System time:               3.37 seconds
```

### Analysis
- ~92.5% of cycles spent in user mode
- ~7.5% of cycles spent in kernel mode
- Better IPC in user mode (1.51) compared to kernel mode (1.24)
- System call overhead slightly higher than reference

## Key Optimization Opportunities
1. **System Call Overhead**
   - Higher system time (3.37s) than reference (1.43s)
   - Could benefit from improved I/O handling

2. **Memory Access Patterns**
   - While improved, LLC misses still higher than reference
   - Potential for better memory locality

3. **Instruction Efficiency**
   - Total instructions still higher than reference
   - Room for further algorithmic improvements

4. **Process Time Distribution**
   - Higher system time percentage indicates I/O bottlenecks
   - Could benefit from better I/O buffering

## Performance Summary
- Total Cycles: ~238.0 billion (1.51x more than reference)
- Total Time: ~59.95 seconds (5.4x slower than reference)
- Dramatic improvement from baseline (15x faster)
- Cache and branch prediction performing well
- Primary remaining bottlenecks are system calls and total instruction count
