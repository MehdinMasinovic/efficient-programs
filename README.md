# Efficient Programs 2024/2025, Group 8

## Efficient Join Implementation Project

## Project Overview
This project involves implementing an efficient join operation across four CSV files, with the goal of optimizing CPU performance. The implementation should match the output of a reference implementation while potentially improving upon its performance characteristics.

## Problem Description
Implement a join operation that:
1. Takes four input CSV files
2. Performs joins based on specific fields:
   - Files 1, 2, and 3 join on their first field
   - File 3's second field joins with File 4's first field
3. Produces output records containing:
   - First field of file4
   - First field of file1
   - Second field of file1
   - Second field of file2
   - Second field of file4

## Reference Implementation
A baseline implementation using Unix commands is provided, taking approximately 155,414,886,890 cycles on the reference machine.

## Test Data
Two test datasets are available:
- Small dataset: a.csv, b.csv, c.csv, d.csv (with reference output abcd.csv)
- Large dataset: f1.csv (355MB), f2.csv (237MB), f3.csv (267MB), f4.csv (326MB)
  - Generates output.csv (703MB)

## Performance Testing
To test performance:
```bash
cd /localtmp/efficient24
LC_NUMERIC=en_US perf stat -e cycles yourjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

To verify output correctness:
```bash
# For small dataset
cd /localtmp/efficient24
yourjoin a.csv b.csv c.csv d.csv|sort|diff - abcd.csv

# For large dataset
cd /localtmp/efficient24
yourjoin f1.csv f2.csv f3.csv f4.csv|sort|diff - output.csv
```

## Key Requirements
- Output must match reference implementation (order can differ)
- Focus is on CPU cycle optimization
- Proper handling of multiplicities in joins
- Records are newline-separated, fields are comma-separated

## Implementation Notes
- Choice of programming language is flexible
- Can use any algorithm or data structure
- Output order is not significant
- CPU time optimization is prioritized over wall clock time

## Project Structure
```
.
├── README.md
├── project-description.pdf
├── reference/
│   ├── myjoin.sh    # Reference bash implementation
│   └── myjoin.pl    # Prolog logical representation
├── data/			# Contains the "small" data (a-d.csv, abcd.csv)
└── src/            # Implementation directory
```