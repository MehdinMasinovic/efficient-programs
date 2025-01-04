# Reference Performance Analysis

## Overview
Analysis of the reference implementation's performance characteristics using `perf stat` with various metrics.

## Basic CPU Metrics

```bash
LC_NUMERIC=en_US perf stat -e cycles,instructions,branches,branch-misses myjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

```
cycles:          157,418,524,786
instructions:    234,499,767,025  (1.49 insn per cycle)
branches:         51,682,095,907
branch-misses:     1,089,370,598  (2.11% of all branches)
Time elapsed:        11.10 seconds
User time:            9.25 seconds
System time:          1.43 seconds
```

### Analysis
- IPC (Instructions Per Cycle) of 1.49 indicates moderate instruction-level parallelism
- Branch misprediction rate of 2.11% is relatively low, suggesting efficient branch prediction
- ~87% of time spent in user mode, ~13% in system calls

## Cache Performance

```bash
LC_NUMERIC=en_US perf stat -e L1-dcache-load-misses,L1-dcache-loads,LLC-load-misses myjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

```
L1-dcache-load-misses:  2,200,132,079  (3.55% of L1-dcache accesses)
L1-dcache-loads:       61,903,382,404
LLC-load-misses:          351,119,363
Time elapsed:              10.64 seconds
User time:                  9.09 seconds
System time:                1.52 seconds
```

### Analysis
- L1 cache miss rate of 3.55% suggests reasonably good cache utilization
- ~351M last-level cache misses indicate significant memory access overhead
- Cache performance might be improved through better data locality

## User vs Kernel Mode Analysis

```bash
LC_NUMERIC=en_US perf stat -e cycles:k,cycles:u,instructions:k,instructions:u myjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

```
cycles (kernel):     12,841,410,432
cycles (user):      142,279,261,810
instructions (kernel): 8,137,015,084  (0.63 insn per cycle)
instructions (user):  226,009,854,642 (1.59 insn per cycle)
Time elapsed:            10.67 seconds
User time:                9.15 seconds
System time:              1.48 seconds
```

### Analysis
- ~92% of cycles spent in user mode
- ~8% of cycles spent in kernel mode
- Better IPC in user mode (1.59) compared to kernel mode (0.63)
- Significant system call overhead suggests potential for optimization

## Key Optimization Opportunities
1. **Memory Access Patterns**
   - High number of cache misses suggests room for improvement in data locality
   - Could benefit from better buffering or streaming strategies

2. **System Call Overhead**
   - ~1.5 seconds spent in system calls
   - Could potentially be reduced through buffering or batch processing

3. **Instruction Efficiency**
   - IPC of 1.49 indicates possible instruction pipeline stalls
   - May benefit from algorithmic optimizations to reduce instruction count

4. **Process Time Distribution**
   - User/System time ratio suggests I/O bound operations
   - Potential for optimization through better I/O handling

## Reference Performance Summary
- Total Cycles: ~157.4 billion
- Total Time: ~11.1 seconds
- Primary bottlenecks appear to be memory access patterns and system call overhead
- Branch prediction and L1 cache performance are relatively good
