# Efficient Programs 2024/2025, Group 8

## Miro Link
https://miro.com/app/board/uXjVLy3w6PI=/

## TL;DR

1. Log into the server provided by the teacher:

   `host: g0.complang.tuwien.ac.at`   
   `user: ep<your student number>`

2. Work on your improvement, then compile the code:

   `g++ -o <name of your file> <name of your file>.cpp`

   e.g. `g++ -o baseline baseline.cpp`

3. Copy [parse_perf_output.py](https://github.com/MehdinMasinovic/efficient-programs/blob/main/src/parse_perf_output.py) and [run_perf_analysis.sh](https://github.com/MehdinMasinovic/efficient-programs/tree/main/src). Also, **ensure that the files have execution permissions** by running:

   `chmod +x parse_perf_output.py run_perf_analysis.sh`

4. Copy the data from the teachers folder into your own so you can run the code:

    `cp /localtmp/efficient24/* .`
   
5. Run the performance analysis on the code:

   `./run_perf_analysis.sh <type of improvement> "<name of your file> f1.csv f2.csv f3.csv f4.csv"`
   
   e.g. `./run_perf_analysis.sh baseline "baseline f1.csv f2.csv f3.csv f4.csv"`

   > The performance analysis will provide you with a CSV file containing the performance metrics. It will be named as `<type of improvement>_performance.csv`, e.g. `baseline_performance.csv`.

6. Ensure the results are correct by comparing it to the teacher's output file:

   `<name of your file> f1.csv f2.csv f3.csv f4.csv | sort | diff - output.csv`
    
   e.g. `baseline f1.csv f2.csv f3.csv f4.csv | sort | diff - output.csv

   If the output of the comparison is empty, then your results are the same.

> Note: Please create a dedicated branch for your improvement, and then create a Pull Request so at least one other person can review the changes.

7. Push the improved code and the performance results to the GitHub project:

   a. The code can be pushed to `efficient-programs/src/<your-code>.cpp`, e.g. `efficient-programs/src/baseline.cpp`

   b. The performance results can be pushed to `efficient-programs/results/<type of improvement>_performance.csv`, e.g. `efficient-programs/results/baseline.csv`

8. Describe in a few key points how your type of improvement actually made the code more efficient. See [the reference implementation performance analysis](https://github.com/MehdinMasinovic/efficient-programs/blob/main/results/reference-performance-analysis.md) as reference. Store the key points in efficient-programs/results/<type of improvement>-performance-analysis.md.

    >Tipp: If you feel lazy, just pass the CSV output of the performance analysis + the markdown of the reference performance analysis to your LLM and ask it to do the same with your data. Simply replace the placeholders `<>` in the following sample prompt:

```txt
Prepare a markdown file explaining the results obtained from the attached <your improvement name>_performance.csv according to the markdown provided in reference-performance-analysis.md. The output markdown should be named <your improvement name>-performance-analysis.md.

<your improvement name>_performance.csv:
<insert the content here>

reference-performance-analysis.md:
<insert the content here>

```

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

> Find a detailed analysis of the reference implementation in `results/baseline-performance-analysis.md`


To test performance:
```bash
cd /localtmp/efficient24
LC_NUMERIC=en_US perf stat -e cycles yourjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

On the test machine provided by the teacher, the performance for the small and large dataset is as follows:

### Small dataset

Command:

```bash
LC_NUMERIC=en_US perf stat -e cycles ~/exc_files/myjoin a.csv b.csv c.csv d.csv|cat >/dev/null
```

Output:

```
31,020,895      cycles                                   
0.016394673 seconds time elapsed
0.000000000 seconds user
0.028339000 seconds sys
```

### Large dataset

Command: 

```bash
LC_NUMERIC=en_US perf stat -e cycles ~/exc_files/myjoin f1.csv f2.csv f3.csv f4.csv|cat >/dev/null
```

Output:

```bash
156,559,163,115      cycles                                   11.242711646 seconds time elapsed
9.090886000 seconds user
1.553000000 seconds sys
```

## Verifying output correctness

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
