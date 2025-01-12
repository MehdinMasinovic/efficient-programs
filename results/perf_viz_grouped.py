import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import glob
from typing import List, Dict, Tuple

# Define implementation order
IMPLEMENTATION_ORDER = [
    'reference', 'baseline', 'hashmap', 'hashmap_size_estim',
    'optimize_io', 'string_encoding', 'string_encoding_2',
    'memory', 'generalImprovementsV4'
]

# List of metrics where lower values indicate better performance
lower_is_better = [
    'cycles', 'time', 'user_time', 'system_time',
    'L1_dcache_load_misses', 'LLC_load_misses',
    'branch_misses', 'branches'
]

def load_performance_files(directory: str) -> pd.DataFrame:
    """Load and combine all performance CSV files from the specified directory."""
    perf_files = glob.glob(str(Path(directory) / "*_performance.csv"))
    if not perf_files:
        raise ValueError(f"No performance CSV files found in {directory}")

    dfs = []
    for file in perf_files:
        if file in ("baseline_performance.csv", "string_encoding_performance.csv"):
            pass
        else:
            df = pd.read_csv(file)
            dfs.append(df)

    return pd.concat(dfs, ignore_index=True)


def convert_value(value: float, unit: str) -> Tuple[float, str]:
    """Convert values to appropriate scale and return the scale suffix."""
    #if unit == 'count':
    #    if value >= 1e9:
    #        return value / 1e9, 'B'
    #    elif value >= 1e6:
    #        return value / 1e6, 'M'
    return value, ''


def calculate_improvement(current: float, reference: float, metric: str) -> str:
    """Calculate percentage improvement relative to reference.
    For metrics where lower is better, inverts the sign of the improvement."""
    pct_change = ((current - reference) / reference) * 100

    return f"{pct_change:+.1f}%"


def create_metric_group_plot(df: pd.DataFrame, group_config: Dict, implementations: List[str]) -> plt.Figure:
    plt.style.use('ggplot')

    # Create figure and primary axis
    fig, ax1 = plt.subplots(figsize=(15, 8))

    # Number of metrics and implementations
    n_metrics = len(group_config['metrics'])
    n_impls = len(implementations)

    # Bar width and positions
    width = 0.8 / n_metrics
    x = np.arange(n_impls)

    # If this is the core performance group, create a secondary axis
    if group_config['title'] == 'Core Performance Metrics':
        ax2 = ax1.twinx()  # Create secondary axis
        axes = [ax1, ax1, ax2]  # Map metrics to axes (cycles->ax1, instructions->ax1, ipc->ax2)
    elif group_config['title'] == 'Branch Prediction':
        ax2 = ax1.twinx()  # Create secondary axis
        axes = [ax2, ax1]  # Map metrics to axes (total branches->ax1, branch misses->ax2)
    elif group_config['title'] == 'Memory Performance':
        ax2 = ax1.twinx()  # Create secondary axis
        axes = [ax1, ax2, ax1]  # Map metrics to axes (L1 Cache Misses->ax1, L1 Cache Loads->ax1, LLC Misses->ax2)
    else:
        axes = [ax1] * len(group_config['metrics'])  # All metrics use primary axis

    # Plot bars for each metric
    # Inside create_metric_group_plot, modify the plotting loop:
    # Plot bars for each metric
    bars = []
    for i, metric_info in enumerate(group_config['metrics']):
        metric = metric_info['name']
        values = []
        scaled_values = []

        # Get values for each implementation
        for impl in implementations:
            value = df[(df['implementation'] == impl) &
                       (df['metric'] == metric)]['value'].iloc[0]
            values.append(value)

            # Scale value
            scaled_value, _ = convert_value(value, df[(df['implementation'] == impl) &
                                                      (df['metric'] == metric)]['unit'].iloc[0])
            scaled_values.append(scaled_value)

        # Plot bars or line based on metric
        pos = x + i * width
        if metric == 'instructions_per_cycle':
            # Plot IPC as a line with diamond markers
            line = axes[i].plot(x + width*2, scaled_values, marker='D', color='#d62728',  # Dark red
                                linestyle='-', linewidth=2.5, markersize=10,
                                label=metric_info['label'])
            bars.append(line[0])

            # Add percentage improvements for IPC slightly higher
            for j in range(1, len(scaled_values)):
                pct = calculate_improvement(values[j], values[0], metric)
                pct_value = float(pct.rstrip('%'))

                # For the color logic:
                is_improvement = (
                    # If lower is better (e.g., cache misses), and value decreased (negative pct)
                        (metric in lower_is_better and pct_value < 0) or
                        # If higher is better (e.g., IPC), and value increased (positive pct)
                        (metric not in lower_is_better and pct_value > 0)
                )
                axes[i].text(x[j] + width*2, scaled_values[j], pct,
                             ha='center', va='bottom', rotation=0,
                             fontsize=8, color='green' if is_improvement else 'red',
                             bbox=dict(facecolor='white', edgecolor='none', alpha=0.7))
        elif metric == 'branches':
            # Plot branches as a line with diamond markers
            line = axes[i].plot(x, scaled_values, marker='D', color='blue',
                                linestyle='-', linewidth=2.5, markersize=10,
                                label=metric_info['label'])
            bars.append(line[0])

            # Add percentage improvements for branches slightly higher
            for j in range(1, len(scaled_values)):
                pct = calculate_improvement(values[j], values[0], metric)
                pct_value = float(pct.rstrip('%'))

                # For the color logic:
                is_improvement = (
                    # If lower is better (e.g., cache misses), and value decreased (negative pct)
                        (metric in lower_is_better and pct_value < 0) or
                        # If higher is better (e.g., IPC), and value increased (positive pct)
                        (metric not in lower_is_better and pct_value > 0)
                )
                axes[i].text(x[j], scaled_values[j], pct,
                             ha='center', va='bottom', rotation=0,
                             fontsize=8, color='green' if is_improvement else 'red',
                             bbox=dict(facecolor='white', edgecolor='none', alpha=0.7))
        elif metric == 'L1_dcache_loads':
            # Plot L1 cache loads as a line with diamond markers
            line = axes[i].plot(x + width, scaled_values, marker='D', color='blue',
                                linestyle='-', linewidth=2.5, markersize=10,
                                label=metric_info['label'])
            bars.append(line[0])

            # Add percentage improvements for IPC slightly higher
            for j in range(1, len(scaled_values)):
                pct = calculate_improvement(values[j], values[0], metric)
                pct_value = float(pct.rstrip('%'))

                # For the color logic:
                is_improvement = (
                    # If lower is better (e.g., cache misses), and value decreased (negative pct)
                        (metric in lower_is_better and pct_value < 0) or
                        # If higher is better (e.g., IPC), and value increased (positive pct)
                        (metric not in lower_is_better and pct_value > 0)
                )
                axes[i].text(x[j] + width, scaled_values[j], pct,
                             ha='center', va='bottom', rotation=0,
                             fontsize=8, color='green' if is_improvement else 'red',
                             bbox=dict(facecolor='white', edgecolor='none', alpha=0.7))

        else:
            # Regular bars for cycles and instructions
            bar = axes[i].bar(pos, scaled_values, width, label=metric_info['label'])
            bars.append(bar)

            # Add percentage improvements for bars
            for j in range(1, len(scaled_values)):
                pct = calculate_improvement(values[j], values[0], metric)
                pct_value = float(pct.rstrip('%'))

                # For the color logic:
                is_improvement = (
                    # If lower is better (e.g., cache misses), and value decreased (negative pct)
                        (metric in lower_is_better and pct_value < 0) or
                        # If higher is better (e.g., IPC), and value increased (positive pct)
                        (metric not in lower_is_better and pct_value > 0)
                )
                axes[i].text(pos[j], scaled_values[j], pct,
                             ha='center', va='bottom', rotation=0,
                             fontsize=8, color='green' if is_improvement else 'red',
                             bbox=dict(facecolor='white', edgecolor='none', alpha=0.7))
    # Customize plot
    ax1.set_title(group_config['title'], pad=20, fontsize=14, fontweight='bold')
    if group_config['title'] == 'Core Performance Metrics':
        ax1.set_ylabel('Cycles and Instructions (in Billions)', fontsize=12)
        ax2.set_ylabel('Instructions Per Cycle (IPC)', fontsize=12)
        # Combine legends from both axes
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2,
                   loc='upper right', bbox_to_anchor=(1.15, 1), fontsize=10)
    elif group_config['title'] == 'Branch Prediction':
        ax1.set_ylabel('Branch Misses (in Billions)', fontsize=12)
        ax2.set_ylabel('Total Branches (in 10 Billions)', fontsize=12)
        # Combine legends from both axes
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2,
                   loc='upper right', bbox_to_anchor=(1.15, 1), fontsize=10)
    elif group_config['title'] == 'Memory Performance':
        ax1.set_ylabel('L1 Cache & LLC Misses (in Billions)', fontsize=12)
        ax2.set_ylabel('L1 Dcache Loads (in 100 Billions)', fontsize=12)
        # Combine legends from both axess
        lines1, labels1 = ax1.get_legend_handles_labels()
        lines2, labels2 = ax2.get_legend_handles_labels()
        ax1.legend(lines1 + lines2, labels1 + labels2,
                   loc='upper right', bbox_to_anchor=(1.15, 1), fontsize=10)
    else:
        ax1.set_ylabel(group_config['ylabel'], fontsize=12)
        ax1.legend(loc='upper right', bbox_to_anchor=(1.15, 1), fontsize=10)

    ax1.set_xticks(x + width * (n_metrics - 1) / 2)
    ax1.set_xticklabels([impl.replace('_', '\n') for impl in implementations],
                        rotation=0, ha='center', fontsize=10)

    # Add grid
    ax1.grid(True, axis='y', linestyle='--', alpha=0.7)

    # Adjust layout
    plt.tight_layout()

    return fig
def main():
    # Define metric groups
    metric_groups = {
        'core_performance': {
            'title': 'Core Performance Metrics',
            'ylabel': 'Value (B: Billions, Ratio for IPC)',
            'metrics': [
                {'name': 'cycles', 'label': 'Cycles (B)'},
                {'name': 'instructions', 'label': 'Instructions (B)'},
                {'name': 'instructions_per_cycle', 'label': 'IPC'}
            ]
        },
        'time': {
            'title': 'Time Performance',
            'ylabel': 'Time (seconds)',
            'metrics': [
                {'name': 'time', 'label': 'Total Time'},
                {'name': 'user_time', 'label': 'User Time'},
                {'name': 'system_time', 'label': 'System Time'}
            ]
        },
        'memory': {
            'title': 'Memory Performance',
            'ylabel': 'Count (B: Billions, M: Millions)',
            'metrics': [
                {'name': 'L1_dcache_load_misses', 'label': 'L1 Cache Misses'},
                {'name': 'L1_dcache_loads', 'label': 'L1 Cache Loads'},
                {'name': 'LLC_load_misses', 'label': 'LLC Misses'}
            ]
        },
        'branch': {
            'title': 'Branch Prediction',
            'ylabel': 'Count (B: Billions, M: Millions)',
            'metrics': [
                {'name': 'branches', 'label': 'Total Branches'},
                {'name': 'branch_misses', 'label': 'Branch Misses'}
            ]
        }
    }

    try:
        # Load and prepare data
        df = load_performance_files(".")

        # Filter available implementations
        available_impls = [impl for impl in IMPLEMENTATION_ORDER
                           if impl in df['implementation'].unique()]

        # Create output directory
        output_dir = "./performance_plots"
        Path(output_dir).mkdir(parents=True, exist_ok=True)

        # Create plots for each metric group
        for group_name, group_config in metric_groups.items():
            fig = create_metric_group_plot(df, group_config, available_impls)
            fig.savefig(f"{output_dir}/{group_name}_metrics.png",
                        dpi=300, bbox_inches='tight')
            plt.close(fig)

        print(f"Successfully generated plots in {output_dir}")

    except Exception as e:
        print(f"Error: {str(e)}")


if __name__ == "__main__":
    main()