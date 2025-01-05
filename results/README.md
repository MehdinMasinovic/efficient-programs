# Implementation Versions Tracking

## Performance Summary Table

| Version | Date | Author | Description | Changes | Performance Improvement |
|---------|------|---------|-------------|----------|----------------------|
| v0.0-reference | 2023-11-30 | Mehdin | Reference Implementation | - Created project<br>- Benchmarked reference implementation<br>- Provided performance analysis script<br>- Analysed performance of reference implementation | - Time: 11.10s<br>- Cycles: 157.4B<br>- IPC: 1.49 |
| v0.1-baseline | 2024-01-04 | Fardokht | Initial implementation | - Basic nested loop join implementation<br>- Simple CSV reading<br>- No optimizations | - Time: 900.49s<br>- Cycles: 4.05T<br>- IPC: 2.54 |
| v0.2-hashmap | 2024-01-04 | Fardokht | Hash-based join implementation | - Replaced nested loops with hash tables<br>- Added unordered_multimap for O(1) lookups<br>- Maintained multiplicity support | - Time: 59.95s<br>- Cycles: 237B<br>- IPC: 1.49 |
| v0.3-mmap | 2024-01-05 | Mehdin | Memory-mapped I/O optimization | - Replaced buffered I/O with memory mapping (mmap) for direct file access<br>- Added direct memory access for CSV parsing<br>- Optimized string handling with assign()<br>- Pre-allocated vectors based on file size<br>- Disabled C/C++ stream synchronization, untyied cout from cin, and set a 1MB custom output buffer for std::cout to reduce system calls during output operations | - Time: 42.93s<br>- Cycles: 167.6B<br>- IPC: 1.14 |
