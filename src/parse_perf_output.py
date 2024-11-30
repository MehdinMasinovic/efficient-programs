#!/usr/bin/env python3
import sys
import re
from typing import Dict, List

def parse_perf_output(perf_output: str) -> List[Dict[str, str]]:
    """Parse perf stat output and return metrics as a list of dictionaries."""
    metrics = []
    
    # Regular expressions for different line formats
    count_pattern = r'^\s*([\d,]+)\s+(\w+(?:[-\w]+)?(?::[ku])?)\s*(?:#\s*([\d.]+)%?\s*(.*))?'
    time_pattern = r'^\s*([\d.]+)\s+seconds\s+(\w+)'
    insn_pattern = r'^\s*([\d,]+)\s+(\w+(?::[ku])?)\s+#\s+([\d.]+)\s+insn per cycle'
    
    # Metric name mappings for consistency
    metric_mappings = {
        'L1-dcache-load-misses': 'L1_dcache_load_misses',
        'L1-dcache-loads': 'L1_dcache_loads',
        'LLC-load-misses': 'LLC_load_misses',
        'branch-misses': 'branch_misses',
        'cycles:k': 'cycles_kernel',
        'cycles:u': 'cycles_user',
        'instructions:k': 'instructions_kernel',
        'instructions:u': 'instructions_user'
    }

    for line in perf_output.strip().split('\n'):
        if not line.strip() or 'Performance counter stats' in line:
            continue
            
        # Try to match instruction per cycle pattern first
        insn_match = re.match(insn_pattern, line)
        if insn_match:
            value_str, metric, ipc = insn_match.groups()
            value = float(value_str.replace(',', ''))
            
            # Handle the instructions metric
            if ':' in metric:
                base_metric, mode = metric.split(':')
                metric_suffix = '_kernel' if mode == 'k' else '_user'
                context = f"{'kernel' if mode == 'k' else 'user'}_mode"
                
                # Add instructions count
                metrics.append({
                    'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                    'metric': f'instructions{metric_suffix}',
                    'value': value,
                    'unit': 'count',
                    'percentage': '',
                    'context': context
                })
                
                # Add IPC
                metrics.append({
                    'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                    'metric': f'ipc{metric_suffix}',
                    'value': float(ipc),
                    'unit': 'ratio',
                    'percentage': '',
                    'context': context
                })
            else:
                # Add total instructions
                metrics.append({
                    'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                    'metric': 'instructions',
                    'value': value,
                    'unit': 'count',
                    'percentage': '',
                    'context': 'total'
                })
                
                # Add total IPC
                metrics.append({
                    'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                    'metric': 'instructions_per_cycle',
                    'value': float(ipc),
                    'unit': 'ratio',
                    'percentage': '',
                    'context': 'total'
                })
            continue
            
        # Try to match count pattern
        count_match = re.match(count_pattern, line)
        if count_match:
            value_str, metric, percentage, context = count_match.groups()
            value = float(value_str.replace(',', ''))
            
            # Skip if this is an instruction line (already handled above)
            if 'instructions' in metric or 'insn per cycle' in line:
                continue
                
            # Normalize metric name and handle special cases
            if ':' in metric:
                base_metric, mode = metric.split(':')
                metric = f"{base_metric}_{'kernel' if mode == 'k' else 'user'}"
                context = f"{'kernel' if mode == 'k' else 'user'}_mode"
            else:
                metric = metric_mappings.get(metric, metric)
                if metric == 'branch_misses':
                    context = 'of_all_branches'
                elif metric == 'L1_dcache_load_misses':
                    context = 'of_L1_accesses'
                else:
                    context = 'total'
            
            metrics.append({
                'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                'metric': metric,
                'value': value,
                'unit': 'count',
                'percentage': percentage if percentage and metric in ['branch_misses', 'L1_dcache_load_misses'] else '',
                'context': context
            })
            continue
            
        # Try to match time pattern
        time_match = re.match(time_pattern, line)
        if time_match:
            value, metric = time_match.groups()
            if metric == 'elapsed':
                metric = 'elapsed_time'
            elif metric == 'user':
                metric = 'user_time'
            elif metric == 'sys':
                metric = 'system_time'
                
            metrics.append({
                'implementation': sys.argv[1] if len(sys.argv) > 1 else 'reference',
                'metric': metric,
                'value': float(value),
                'unit': 'seconds',
                'percentage': '',
                'context': 'total'
            })
    
    return metrics

def write_csv(metrics: List[Dict[str, str]], output_file=None):
    """Write metrics to CSV file."""
    headers = ['implementation', 'metric', 'value', 'unit', 'percentage', 'context']
    
    # Write to stdout if no output file specified
    f = sys.stdout if output_file is None else open(output_file, 'w')
    
    # Write headers
    f.write(','.join(headers) + '\n')
    
    # Write metrics
    for metric in metrics:
        row = [str(metric.get(header, '')) for header in headers]
        f.write(','.join(row) + '\n')
    
    if output_file:
        f.close()

def main():
    """Main function to read perf output and generate CSV."""
    if len(sys.argv) < 2:
        print("Usage: python parse_perf_output.py <implementation_name>")
        sys.exit(1)
        
    # Read perf output from stdin
    perf_output = sys.stdin.read()
    
    # Parse metrics
    metrics = parse_perf_output(perf_output)
    
    # Write to stdout
    write_csv(metrics)

if __name__ == "__main__":
    main()
