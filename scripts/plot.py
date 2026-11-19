import os
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

results_csv = "results/results.csv"
tradeoff_csv = "results/tradeoff_k.csv"
plots_dir = "plots"
os.makedirs(plots_dir, exist_ok=True)

cpu_cores = int(os.environ.get("NUM_CORES", os.cpu_count() or 4))

# Read benchmark results and compute mean and std
df = pd.read_csv(results_csv)
grouped = df.groupby(["approach", "threads"])["time_sec"].agg(["mean", "std"]).reset_index()

# Baseline single-thread execution time
single_rows = grouped[grouped["approach"] == "single"]
if not single_rows.empty:
    t_single = single_rows["mean"].values[0]
else:
    t_single = grouped[grouped["threads"] == 1]["mean"].values[0]

# Compute speedup and parallel efficiency
grouped["speedup"] = t_single / grouped["mean"]
grouped["efficiency"] = grouped["speedup"] / grouped["threads"]

print("=== Summary Results ===")
print(grouped.to_string(index=False))

threads_ticks = sorted(grouped["threads"].unique())

# 1. Execution Time vs Threads
plt.figure(figsize=(8, 5))
for app in grouped["approach"].unique():
    sub = grouped[grouped["approach"] == app].sort_values("threads")
    plt.errorbar(sub["threads"], sub["mean"], yerr=sub["std"].fillna(0), marker="o", capsize=4, label=app)
plt.axvline(x=cpu_cores, color="red", linestyle="--", label=f"CPU Cores ({cpu_cores})")
plt.xscale("log", base=2)
plt.xticks(threads_ticks, [str(t) for t in threads_ticks])
plt.xlabel("Number of Threads (log2 scale)")
plt.ylabel("Mean Execution Time (s)")
plt.title("Execution Time vs. Thread Count")
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "time_vs_threads.png"), dpi=150)
plt.close()

# 2. Speedup vs Threads
plt.figure(figsize=(8, 5))
for app in grouped["approach"].unique():
    sub = grouped[grouped["approach"] == app].sort_values("threads")
    plt.plot(sub["threads"], sub["speedup"], marker="s", label=app)
ideal_x = sorted(set([t for t in threads_ticks if t <= cpu_cores] + [cpu_cores]))
plt.plot(ideal_x, ideal_x, "k:", label="Ideal Linear")
plt.axvline(x=cpu_cores, color="red", linestyle="--", label=f"CPU Cores ({cpu_cores})")
plt.xscale("log", base=2)
plt.xticks(threads_ticks, [str(t) for t in threads_ticks])
plt.xlabel("Number of Threads (log2 scale)")
plt.ylabel("Speedup (T_single / T_n)")
plt.title("Parallel Speedup vs. Thread Count")
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "speedup_vs_threads.png"), dpi=150)
plt.close()

# 3. Parallel Efficiency vs Threads
plt.figure(figsize=(8, 5))
for app in grouped["approach"].unique():
    sub = grouped[grouped["approach"] == app].sort_values("threads")
    plt.plot(sub["threads"], sub["efficiency"], marker="^", label=app)
plt.axhline(y=1.0, color="gray", linestyle=":", label="Ideal (100%)")
plt.axvline(x=cpu_cores, color="red", linestyle="--", label=f"CPU Cores ({cpu_cores})")
plt.xscale("log", base=2)
plt.xticks(threads_ticks, [str(t) for t in threads_ticks])
plt.xlabel("Number of Threads (log2 scale)")
plt.ylabel("Parallel Efficiency (Speedup / n)")
plt.title("Parallel Efficiency vs. Thread Count")
plt.grid(True, linestyle=":", alpha=0.6)
plt.legend()
plt.tight_layout()
plt.savefig(os.path.join(plots_dir, "efficiency_vs_threads.png"), dpi=150)
plt.close()

# 4. Comparison: Local Accumulation vs Mutex (Experiment 6.3)
local_df = grouped[grouped["approach"] == "local"].sort_values("threads")
mutex_df = grouped[grouped["approach"] == "mutex"].sort_values("threads")
if not local_df.empty and not mutex_df.empty:
    plt.figure(figsize=(8, 5))
    plt.plot(local_df["threads"], local_df["mean"], marker="o", label="local")
    plt.plot(mutex_df["threads"], mutex_df["mean"], marker="x", label="mutex")
    plt.axvline(x=cpu_cores, color="red", linestyle="--", label=f"CPU Cores ({cpu_cores})")
    plt.xscale("log", base=2)
    plt.xticks(threads_ticks, [str(t) for t in threads_ticks])
    plt.xlabel("Number of Threads (log2 scale)")
    plt.ylabel("Mean Execution Time (s)")
    plt.title("Experiment 6.3: Local Accumulation vs. Mutex")
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "comparison_local_vs_mutex.png"), dpi=150)
    plt.close()

# 5. Experiment 6.1: Execution Time vs Batch Size k (log scale)
if os.path.exists(tradeoff_csv):
    df_k = pd.read_csv(tradeoff_csv)
    df_k["k_val"] = df_k["approach"].str.replace("mutex_k", "", regex=False).astype(int)
    gk = df_k.groupby("k_val")["time_sec"].agg(["mean", "std"]).reset_index().sort_values("k_val")

    plt.figure(figsize=(8, 5))
    plt.errorbar(gk["k_val"], gk["mean"], yerr=gk["std"].fillna(0), marker="o", capsize=4)
    plt.xscale("log")
    plt.xlabel("Batch Size k (log scale)")
    plt.ylabel("Mean Execution Time (s)")
    plt.title("Experiment 6.1: Execution Time vs. Batch Size k")
    plt.grid(True, which="both", linestyle=":", alpha=0.6)
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "tradeoff_k.png"), dpi=150)
    plt.close()

print("Plots saved to plots/")
