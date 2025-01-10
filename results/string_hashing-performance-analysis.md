# String Hashing Performance Analysis

## Overview

Analysis of the `string_hashing` implementation's performance characteristics using `perf stat` with various metrics.

## Idea

The fundamental idea was to encoded strings as numbers to reduce the number of comparison checks for equivalence.
More detailed pieces of information can be found in the source file.

## Basic CPU Metrics

| Metric | Result |
| --- | --- |
| cycles | 160 367 984 920 |
| instructions | 198 107 582 844 |
| IPC | 1.24 |
| branches | 42 333 550 817 |
| branch-misses | 380 248 013 (0.9 %) |
| time elapsed | 39.86 s |
| user time | 37.49 s |
| system time | 2.38 s |

### Analysis

- slight decrease in the number of cycles in contrast to `optimizeio`, but huge decrease in regards to the custom string encoding done in `string_encoding`
- slight increase in number of instruction comparing to `optimizeio`
- lower IPC then both `reference` and `string_encoding`, but slightly better than `optimizeio`
- almost equal branches, but less branch misses than `optimizeio`
- shorter runtime than `optimizeio`, but almost 4x the runtime as the `reference` implementation

## Cache Performance

| Metric | Result |
| --- | --- |
| L1-dcache-misses | 950 333 066 (1.46 %) |
| L1-dcache-loads | 65 126 394 082 |
| LLC-load-misses | 246 551 106 |

### Analysis

- comparing `optimizeio` this implementation has fewer cache misses and higher cache loads, which means a better cache utilization overall
- lower LLC misses in comparison to `optimizeio`
- increase in cache misses in regards to `string_encoding`

## User vs Kernel Mode Analysis

| Metric | Result |
| --- | --- |
| cycles (kernel) | 12 470 043 080 |
| cycles (user) | 147 904 068 371 |
| IPC (kernel) | 1.23 |
| IPC (user) | 1.24 |

### Analysis

- almost identical cycle numbers to `optimizeio`
- huge decrease in cycles in regards to `string_endcoding`
- decrease in IPC for the user mode in contrast to `string_encoding` but not for `optimizeio`

## Overall Assessment

Simply hashing the values has a tremendeous effect on the performance.
But maybe using pointers in the whole algorithm is even faster.