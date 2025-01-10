# String Encoding Performance Analysis

## Overview

Analysis of the `string_encoding` implementation's performance characteristics using `perf stat` with various metrics.

## Idea

The fundamental idea was to encoded strings as numbers to reduce the number of comparison checks for equivalence.
More detailed pieces of information can be found in the source file.

## Basic CPU Metrics

| Metric | Result |
| --- | --- |
| cycles | 3 477 608 101 315 |
| instructions | 5 801 907 903 772 |
| IPC | 1.67 |
| branches | 1 248 915 862 902 |
| branch-misses | 2 419 562 600 (0.19 %) |
| time elapsed | 903.57 s (timeout) |
| user time | 867.42 s |
| system time | 33.12 s |

### Analysis

- This implementation is a step back from its predecessor the `optimizeio` implementation.
- There is an enormus increase in cycles and instructions.
- There is an immense increase in branches and branch misses. I would theorize that this is one of the leading factors for the increase in cylces and decrease in performance.

## Cache Performance

| Metric | Result |
| --- | --- |
| L1-dcache-misses | 20 547 971 348 (1.04 %) |
| L1-dcache-loads | 1 970 709 464 078 |
| LLC-load-misses | 9 492 547 830 |

### Analysis

- L1 cache miss rate (1.04 %) is better than the `optimizeio` implementation
- There are a lot more cache loads than in the `optimizeio` or `reference` implementation
- The LLC misses increased by almost a factor of 10 in comparison to `optimizeio`

## User vs Kernel Mode Analysis

| Metric | Result |
| --- | --- |
| cycles (kernel) | 141 105 533 992 |
| cycles (user) | 3 340 195 715 171 |
| IPC (kernel) | 0.94 |
| IPC (user) | 1.7 |

### Analysis

- the IPC in kernel mode is lower than the `optimizeio` implementation
- the IPC in user mode is better than the `optimizeio` implementation

## Overall Assessment

The implementation of the string encoding was not beneficial.
In regards to almost all metrics this implementation is worse and has no real performance increase.
