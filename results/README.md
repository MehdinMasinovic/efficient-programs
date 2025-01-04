# Implementation Versions Tracking

## Performance Summary Table

| Version | Date | Author | Description | Changes | Performance Improvement |
|---------|------|---------|-------------|----------|----------------------|
| v0.0-reference | 2023-11-30 | Mehdin | Reference Implementation | - Created project<br>- Benchmarked reference implementation<br>- Provided performance analysis script<br>- Analysed performance of reference implementation | Reference:<br>- Time: 11.10s<br>- Cycles: 157.4B<br>- IPC: 1.49 |
| v0.1-baseline | 2024-01-04 | Fardokht | Initial implementation | - Basic nested loop join implementation<br>- Simple CSV reading<br>- No optimizations | Baseline:<br>- Time: 900.49s<br>- Cycles: 4.05T<br>- IPC: 2.54 |
| v0.2-hashmap | 2024-01-04 | Fardokht | Hash-based join implementation | - Replaced nested loops with hash tables<br>- Added unordered_multimap for O(1) lookups<br>- Maintained multiplicity support | vs Baseline:<br>- 15x faster (59.95s)<br>- 17x fewer cycles<br>- Better cache usage (1.32% vs 2.64% L1 miss rate) |

