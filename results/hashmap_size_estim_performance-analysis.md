# Hashmap Size Estimation Analysis

## Overview
Analysis of the hashmap-based implementation's performance characteristics when using reserve for allocating the memory using `perf stat` with various metrics.

## Basic CPU Metrics

```
cycles:          216,579,545,767
instructions:    352,580,039,598  (1.63 insn per cycle)
branches:         74,082,967,708
branch-misses:       450,258,564  (0.61% of all branches)
Time elapsed:         52.20 seconds
User time:            49.08 seconds
System time:           3.12 seconds
```

### Analysis
- Branch misprediction rate of 0.61% is lower than reference (2.11%)
- ~94% of time spent in user mode, ~6% in system calls

## Cache Performance

```
L1-dcache-load-misses:   1,251,644,311  (1.12% of L1-dcache accesses)
L1-dcache-loads:       111,642,094,162
LLC-load-misses:           343,184,193
Time elapsed:              52.20 seconds
User time:                 49.08 seconds
System time:                3.12 seconds
```

### Analysis
- L1 cache miss rate of 1.12% is better than reference (3.55%), baseline (2.64%) and hashmap (1.32%)
- LLC misses (343M) are better than reference (351M)

## User vs Kernel Mode Analysis

```
cycles (kernel):     16,465,664,455
cycles (user):      200,087,099,533
instructions (kernel): 20,418,639,874  (1.24 insn per cycle)
instructions (user):  332,268,170,096  (1.66 insn per cycle)
Time elapsed:             52.20 seconds
User time:                49.08 seconds
System time:               3.12 seconds
```

### Analysis
- ~92.4% of cycles spent in user mode
- ~7.6% of cycles spent in kernel mode
- Better IPC in user mode (1.66) compared to kernel mode (1.24)

## Key Optimization Opportunities
1. **System Call Overhead**
   - Higher system time (3.12s) than reference (1.43s)
   - Could benefit from improved I/O handling

2. **Instruction Efficiency**
   - Total instructions still higher than reference
   - Room for further algorithmic improvements

3. **Process Time Distribution**
   - Higher system time percentage indicates I/O bottlenecks
   - Could benefit from better I/O buffering

## Performance Summary
- Total Cycles: ~217.0 billion (1.40x more than reference)
- Total Time: ~52.20 seconds (4.9x slower than reference)
- Small improvement from hashmap (1.15x faster)
- Cache and branch prediction performing well
- Primary remaining bottlenecks are system calls and total instruction count
