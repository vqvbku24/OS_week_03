#!/usr/bin/env python3
"""
scripts/plot.py - Data Processing and Visualization for Monte Carlo Pi Lab
Assigned Member: Member E
"""

import os
import sys

def main():
    try:
        import pandas as pd
        import matplotlib.pyplot as plt
    except ImportError as e:
        print(f"[plot.py] Missing dependency: {e}")
        print("Please install requirements: pip install pandas matplotlib")
        sys.exit(1)

    results_csv = "results/results.csv"
    tradeoff_csv = "results/tradeoff_k.csv"
    plots_dir = "plots"
    os.makedirs(plots_dir, exist_ok=True)

    if not os.path.exists(results_csv):
        print(f"[plot.py] Warning: File not found: {results_csv}. Please run benchmarks first ('make bench').")
        return

    # TODO(E-7): Read results.csv, compute mean and standard deviation grouped by (approach, threads) (Member E)
    df = pd.read_csv(results_csv)
    if df.empty:
        print("[plot.py] Error: results.csv is empty.")
        return

    print(f"[plot.py] Read {len(df)} records from {results_csv}.")

    # Group by approach and threads, compute mean and std
    grouped = df.groupby(["approach", "threads"])["time_sec"].agg(["mean", "std"]).reset_index()

    # Determine baseline single-threaded execution time
    single_rows = grouped[grouped["approach"] == "single"]
    if not single_rows.empty:
        t_single = single_rows["mean"].values[0]
    else:
        # Fallback to 1-thread execution of first available approach
        t_single = grouped[grouped["threads"] == 1]["mean"].values[0] if not grouped[grouped["threads"] == 1].empty else 1.0

    print(f"[plot.py] Baseline execution time T_single: {t_single:.6f}s")

    # TODO(E-8): Compute Speedup = T_single / T_parallel(n); Efficiency = Speedup / n (Member E)
    grouped["speedup"] = t_single / grouped["mean"]
    grouped["efficiency"] = grouped["speedup"] / grouped["threads"]

    # TODO(E-11): Detect system CPU core count to plot reference vertical line (Member E)
    cpu_cores = os.cpu_count() or 4

    # TODO(E-9): Generate 3 graphs (time, speedup, efficiency) and overall comparison plot in plots/ (Member E)

    # 1. Execution Time Plot
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.errorbar(sub["threads"], sub["mean"], yerr=sub["std"].fillna(0), marker='o', label=app, capsize=4)
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'CPU Cores ({cpu_cores})')
    plt.xlabel("Number of Threads", fontsize=12)
    plt.ylabel("Mean Execution Time (seconds)", fontsize=12)
    plt.title("Execution Time vs. Thread Count", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "time_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Generated: plots/time_vs_threads.png")

    # 2. Speedup Plot
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.plot(sub["threads"], sub["speedup"], marker='s', label=app)
    # Ideal linear speedup reference line
    max_th = grouped["threads"].max()
    plt.plot([1, min(max_th, cpu_cores)], [1, min(max_th, cpu_cores)], 'k:', label='Ideal Linear Speedup')
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'CPU Cores ({cpu_cores})')
    plt.xlabel("Number of Threads", fontsize=12)
    plt.ylabel("Speedup (T_single / T_parallel)", fontsize=12)
    plt.title("Parallel Speedup vs. Thread Count", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "speedup_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Generated: plots/speedup_vs_threads.png")

    # 3. Parallel Efficiency Plot
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.plot(sub["threads"], sub["efficiency"], marker='^', label=app)
    plt.axhline(y=1.0, color='gray', linestyle=':', label='100% Efficiency')
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'CPU Cores ({cpu_cores})')
    plt.xlabel("Number of Threads", fontsize=12)
    plt.ylabel("Parallel Efficiency (Speedup / n)", fontsize=12)
    plt.title("Parallel Efficiency vs. Thread Count", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "efficiency_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Generated: plots/efficiency_vs_threads.png")

    # 4. Direct Comparison Plot: Local Accumulation vs Mutex (Experiment 6.3)
    local_df = grouped[grouped["approach"] == "local"].sort_values("threads")
    mutex_df = grouped[grouped["approach"] == "mutex"].sort_values("threads")
    if not local_df.empty and not mutex_df.empty:
        plt.figure(figsize=(10, 6))
        plt.plot(local_df["threads"], local_df["mean"], marker='o', color='blue', label='Approach 2: Local Accumulation')
        plt.plot(mutex_df["threads"], mutex_df["mean"], marker='x', color='red', label='Approach 3: Shared Mutex')
        plt.axvline(x=cpu_cores, color='gray', linestyle='--', label=f'CPU Cores ({cpu_cores})')
        plt.xlabel("Number of Threads", fontsize=12)
        plt.ylabel("Mean Execution Time (seconds)", fontsize=12)
        plt.title("Experiment 6.3: Local Accumulation vs. Shared Mutex", fontsize=14)
        plt.grid(True, linestyle=":", alpha=0.6)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(plots_dir, "comparison_local_vs_mutex.png"), dpi=150)
        plt.close()
        print("  -> Generated: plots/comparison_local_vs_mutex.png")

    # TODO(E-10): Plot 6.1: Execution time vs k batch size (log scale) (Member E)
    if os.path.exists(tradeoff_csv):
        df_k = pd.read_csv(tradeoff_csv)
        if not df_k.empty:
            df_k["k_val"] = df_k["approach"].apply(
                lambda x: int(x.replace("mutex_k", "")) if "mutex_k" in str(x) else 1
            )
            gk = df_k.groupby("k_val")["time_sec"].agg(["mean", "std"]).reset_index().sort_values("k_val")

            plt.figure(figsize=(10, 6))
            plt.errorbar(gk["k_val"], gk["mean"], yerr=gk["std"].fillna(0), marker='o', capsize=5, color='darkorange', linewidth=2)
            plt.xscale("log")
            plt.xlabel("Batch Synchronization Frequency k (Log scale)", fontsize=12)
            plt.ylabel("Mean Execution Time (seconds)", fontsize=12)
            plt.title("Experiment 6.1: Synchronization Trade-off vs. Batch Size k", fontsize=14)
            plt.grid(True, which="both", linestyle=":", alpha=0.6)
            plt.tight_layout()
            plt.savefig(os.path.join(plots_dir, "tradeoff_k.png"), dpi=150)
            plt.close()
            print("  -> Generated: plots/tradeoff_k.png")

    print("[plot.py] All plots have been successfully generated!")

if __name__ == "__main__":
    main()
