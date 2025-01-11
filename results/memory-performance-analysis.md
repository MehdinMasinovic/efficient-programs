# Memory Optimizations Analyis

## Overview

Analysis of the `memory` implementation's performance and analysis of not implmented ideas that did either not work or did not improve performance.

## Idea

The goal was to improve memory management with a special focus on caching. The 4 main optimization ideas included:
- Prefetching data for the cache (implemented)
- Aligning mapping size to system page boundaries (implemented)
- Improving memory locality by introducing C-style char arrays (not implemented)
- Improving memory usage by using process streaming (not implemented)

## Metrics

| Metric | Result |
| --- | --- |
| cycles | 145 444 099 941 |
| instructions | 192 535 510 736 |
| IPC | 1.32 |
| branches | 41 114 985 124 |
| branch-misses | 385 156 733 (0.94 %) |
| L1 dcache misses | 952 550 911 (1.5 %) |
| L1 dcache loads | 63 605 153 803 |
| LLC load misses | 236 396 775 |
| time elapsed | 36.06 s |
| user time | 34.19 s |
| system time | 1.86 s |

### Analysis

- around 15 billion cycles lower compared to `string-hashing`
- instructions got decreased by ~ 6 billion compared to `string-hashing`
- IPC increased slightly
- slightly more branch misses
- more cache misses (~ 0.04 %) funnily enough (could just be fluctuations on g0)
- runtime decreased by around 3 seconds

## Idea Analysis

### Prefetching

Prefetching saved around 2-3 billion cycles, but this seems to heavily fluctuate based on the current state of the memory. With the introduction of more significant optimizations (regarding memory locality), this could lead to a decrease in performance due to heavy preloading of the cache even if there is no cache miss.

### Mapping size alignment

This significantly reduced the cycle count by around 12 billion cycles. By only mapping the data in sizes that fit within pages of the system, a decrease in TLB (Translation Lookahead Buffer) misses can be achieved which directly correlates to less memory accesses and, therefore, cycles. This optimization, however, also heavily relies on the state of the system, as e.g. optimal loading conditions in memory could make the optimization obsolete.

### Char arrays

This optimization would have been the most promising one (a decrease of around 25 billion cycles compared to `string-hashing`) but, unfortunately, the join files provided include a single null byte that is included in the join (I don't know if this intentional of the teacher or just a plain mistake) which makes working with C-style char arrays (which are null byte terminated) impossible (at least not without the introduction of a significant overhead which would make the optimization meaningless).

### Process Streaming

This was purely a speculative optimazation that would have probably worked if the files were even bigger (more than 1 GB per file) but introduced such a high overhead, that it decreased performance significantly. The idea was to make more concise chunk sizes. So instead of loading the whole file in memory, one would only load it chunk by chunk which could reduce the lookup in memory. For this task, it did not succeed.

## Overall Assessment

The mapping size alignment to the page size was very significant. The prefetching of data also increased performance but this could also become insignificant (or even worsen performance) if better optimizations regarding memory locality are introduced.

