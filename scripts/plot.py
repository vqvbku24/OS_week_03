#!/usr/bin/env python3
"""
scripts/plot.py - Xu ly so lieu va ve do thi cho bao cao Monte Carlo Pi Lab
Nguoi phu trach: Thanh vien E
"""

import os
import sys

def main():
    try:
        import pandas as pd
        import matplotlib.pyplot as plt
    except ImportError as e:
        print(f"[plot.py] Loi thieu thu vien: {e}")
        print("Vui long cai dat: pip install pandas matplotlib")
        sys.exit(1)

    results_csv = "results/results.csv"
    tradeoff_csv = "results/tradeoff_k.csv"
    plots_dir = "plots"
    os.makedirs(plots_dir, exist_ok=True)

    if not os.path.exists(results_csv):
        print(f"[plot.py] Canh bao: Chua co file {results_csv}. Vui long chay benchmark truoc ('make bench').")
        return

    # TODO(E-7): Doc results.csv, tinh trung binh va do lech chuan theo (approach, threads) (Thanh vien E)
    df = pd.read_csv(results_csv)
    if df.empty:
        print("[plot.py] File results.csv rong.")
        return

    print(f"[plot.py] Da doc {len(df)} dong du lieu tu {results_csv}.")

    # Gom nhom va tinh mean, std
    grouped = df.groupby(["approach", "threads"])["time_sec"].agg(["mean", "std"]).reset_index()

    # Lay baseline thoi gian single thread
    single_rows = grouped[grouped["approach"] == "single"]
    if not single_rows.empty:
        t_single = single_rows["mean"].values[0]
    else:
        # Neu chua co single thread thi lay thoi gian 1 thread cua approach dau tien lam moc
        t_single = grouped[grouped["threads"] == 1]["mean"].values[0] if not grouped[grouped["threads"] == 1].empty else 1.0

    print(f"[plot.py] Thoi gian baseline T_single: {t_single:.6f}s")

    # TODO(E-8): Tinh Speedup = T_single / T_parallel(n); Efficiency = Speedup / n (Thanh vien E)
    grouped["speedup"] = t_single / grouped["mean"]
    grouped["efficiency"] = grouped["speedup"] / grouped["threads"]

    # TODO(E-11): Xac dinh so core CPU de ve duong thang dung tham chieu tren cac do thi (Thanh vien E)
    cpu_cores = os.cpu_count() or 4

    # TODO(E-9): Ve 3 do thi (time, speedup, efficiency) va 1 do thi so sanh chung; luu vao plots/ (Thanh vien E)

    # 1. Do thi Thoi gian thuc thi (Execution Time)
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.errorbar(sub["threads"], sub["mean"], yerr=sub["std"].fillna(0), marker='o', label=app, capsize=4)
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'So core CPU ({cpu_cores})')
    plt.xlabel("Số luồng (Threads)", fontsize=12)
    plt.ylabel("Thời gian thực thi trung bình (giây)", fontsize=12)
    plt.title("Thời gian thực thi theo số luồng", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "time_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Da xuat: plots/time_vs_threads.png")

    # 2. Do thi Speedup
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.plot(sub["threads"], sub["speedup"], marker='s', label=app)
    # Ve duong speedup ly tuong (ideal linear speedup)
    max_th = grouped["threads"].max()
    plt.plot([1, min(max_th, cpu_cores)], [1, min(max_th, cpu_cores)], 'k:', label='Tuyến tính lý tưởng (Linear)')
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'So core CPU ({cpu_cores})')
    plt.xlabel("Số luồng (Threads)", fontsize=12)
    plt.ylabel("Speedup (T_single / T_parallel)", fontsize=12)
    plt.title("Tốc độ tăng tốc (Speedup) theo số luồng", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "speedup_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Da xuat: plots/speedup_vs_threads.png")

    # 3. Do thi Parallel Efficiency
    plt.figure(figsize=(10, 6))
    for app in grouped["approach"].unique():
        sub = grouped[grouped["approach"] == app].sort_values("threads")
        plt.plot(sub["threads"], sub["efficiency"], marker='^', label=app)
    plt.axhline(y=1.0, color='gray', linestyle=':', label='100% Efficiency')
    plt.axvline(x=cpu_cores, color='red', linestyle='--', label=f'So core CPU ({cpu_cores})')
    plt.xlabel("Số luồng (Threads)", fontsize=12)
    plt.ylabel("Hiệu suất song song (Parallel Efficiency = Speedup / n)", fontsize=12)
    plt.title("Hiệu suất song song theo số luồng", fontsize=14)
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, "efficiency_vs_threads.png"), dpi=150)
    plt.close()
    print("  -> Da xuat: plots/efficiency_vs_threads.png")

    # 4. Do thi Thi nghiem 6.3: So sanh truc tiep Local Accumulation vs Mutex
    local_df = grouped[grouped["approach"] == "local"].sort_values("threads")
    mutex_df = grouped[grouped["approach"] == "mutex"].sort_values("threads")
    if not local_df.empty and not mutex_df.empty:
        plt.figure(figsize=(10, 6))
        plt.plot(local_df["threads"], local_df["mean"], marker='o', color='blue', label='Approach 2: Local Accumulation')
        plt.plot(mutex_df["threads"], mutex_df["mean"], marker='x', color='red', label='Approach 3: Shared Mutex')
        plt.axvline(x=cpu_cores, color='gray', linestyle='--', label=f'So core CPU ({cpu_cores})')
        plt.xlabel("Số luồng (Threads)", fontsize=12)
        plt.ylabel("Thời gian thực thi (giây)", fontsize=12)
        plt.title("Thí nghiệm 6.3: So sánh Local Accumulation vs Mutex", fontsize=14)
        plt.grid(True, linestyle=":", alpha=0.6)
        plt.legend()
        plt.tight_layout()
        plt.savefig(os.path.join(plots_dir, "comparison_local_vs_mutex.png"), dpi=150)
        plt.close()
        print("  -> Da xuat: plots/comparison_local_vs_mutex.png")

    # TODO(E-10): Do thi 6.1: Thoi gian theo k (truc log) (Thanh vien E)
    if os.path.exists(tradeoff_csv):
        df_k = pd.read_csv(tradeoff_csv)
        if not df_k.empty:
            # Trich xuat so k tu cot approach dang mutex_k<k>
            df_k["k_val"] = df_k["approach"].apply(
                lambda x: int(x.replace("mutex_k", "")) if "mutex_k" in str(x) else 1
            )
            gk = df_k.groupby("k_val")["time_sec"].agg(["mean", "std"]).reset_index().sort_values("k_val")

            plt.figure(figsize=(10, 6))
            plt.errorbar(gk["k_val"], gk["mean"], yerr=gk["std"].fillna(0), marker='o', capsize=5, color='darkorange', linewidth=2)
            plt.xscale("log")
            plt.xlabel("Chu kỳ đồng bộ k (Log scale)", fontsize=12)
            plt.ylabel("Thời gian thực thi trung bình (giây)", fontsize=12)
            plt.title("Thí nghiệm 6.1: Đánh đổi thời gian theo chu kỳ đồng bộ k", fontsize=14)
            plt.grid(True, which="both", linestyle=":", alpha=0.6)
            plt.tight_layout()
            plt.savefig(os.path.join(plots_dir, "tradeoff_k.png"), dpi=150)
            plt.close()
            print("  -> Da xuat: plots/tradeoff_k.png")

    print("[plot.py] Hoan tat xu ly va ve tat ca do thi!")

if __name__ == "__main__":
    main()
