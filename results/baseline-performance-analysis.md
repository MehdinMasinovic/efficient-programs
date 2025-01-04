# Baseline Performance Analysis

## Overview
Analysis of the baseline implementation's performance characteristics using `perf stat` with various metrics.

## Basic CPU Metrics

```
cycles:          4,045,288,369,850
instructions:    10,283,720,143,596  (2.54 insn per cycle)
branches:        2,326,592,041,270
branch-misses:   6,263,808,367  (0.27% of all branches)
Time elapsed:    900.49 seconds
User time:       898.05 seconds
System time:     2.08 seconds
```

### Analysis
- IPC (Instructions Per Cycle) of 2.54 indicates good instruction-level parallelism
- Branch misprediction rate of 0.27% is exceptionally low
- ~99.7% of time spent in user mode, ~0.3% in system calls

## Cache Performance

```
L1-dcache-load-misses:  100,759,817,491  (2.64% of L1-dcache accesses)
L1-dcache-loads:      3,818,199,479,053
LLC-load-misses:         5,414,244,334
Time elapsed:              900.49 seconds
User time:                 898.05 seconds
System time:                 2.08 seconds
```

### Analysis
- L1 cache miss rate of 2.64% suggests good cache utilization
- ~5.4B last-level cache misses indicate significant memory access overhead
- Cache performance might be improved through better data locality

## User vs Kernel Mode Analysis

```
cycles (kernel):        12,112,354,722
cycles (user):       4,033,118,942,057
instructions (kernel):   14,049,613,471  (1.16 insn per cycle)
instructions (user): 10,270,127,829,751  (2.55 insn per cycle)
Time elapsed:              900.49 seconds
User time:                 898.05 seconds
System time:                 2.08 seconds
```

### Analysis
- ~99.7% of cycles spent in user mode
- ~0.3% of cycles spent in kernel mode
- Better IPC in user mode (2.55) compared to kernel mode (1.16)
- Minimal system call overhead

## Key Optimization Opportunities
1. **Overall Performance**
   - Total cycles (4.04T) are significantly higher than reference (157.4B)
   - Runtime of 900s vs reference 11s indicates major algorithmic inefficiency

2. **Instruction Efficiency**
   - Despite good IPC (2.54), total instruction count is 43x higher than reference
   - Suggests need for fundamental algorithmic improvements

3. **Memory Access Patterns**
   - Higher LLC misses (5.4B vs 351M) indicate suboptimal memory access
   - Could benefit from better data structure organization

4. **Process Time Distribution**
   - Almost entirely CPU-bound (99.7% user time)
   - Indicates algorithmic complexity is the primary bottleneck

## Baseline Performance Summary
- Total Cycles: ~4.04 trillion (25.7x more than reference)
- Total Time: ~900.5 seconds (81x slower than reference)
- Primary bottleneck is algorithmic efficiency (nested loops vs. hash joins)
- Good low-level metrics (IPC, branch prediction) masked by poor algorithmic complexity
