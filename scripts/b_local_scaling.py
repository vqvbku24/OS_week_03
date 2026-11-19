
#!/usr/bin/env python3

"""
Member B - Local Accumulation Benchmark
Measures execution time, speedup, efficiency,
context switches and false sharing.
"""

import csv
import os
import resource
import statistics
import subprocess
import sys

N = int(float(os.environ.get("N", "1e8")))
RUNS = int(os.environ.get("RUNS", "5"))
SEED = os.environ.get("SEED", "12345")

THREADS = [
    int(t) for t in os.environ.get(
        "THREADS",
        "1 2 3 4 8 16 32 64 100 128 256 512 1024"
    ).split()
]

BIN = "bin"
FS_BIN = os.path.join(BIN, "false_sharing_demo")


def run(cmd):
    before = resource.getrusage(resource.RUSAGE_CHILDREN)

    out = subprocess.run(
        cmd,
        check=True,
        capture_output=True,
        text=True
    ).stdout.strip()

    after = resource.getrusage(resource.RUSAGE_CHILDREN)

    fields = out.splitlines()[-1].split(",")

    voluntary = after.ru_nvcsw - before.ru_nvcsw
    involuntary = after.ru_nivcsw - before.ru_nivcsw

    return fields, voluntary, involuntary


def summarize(rows, key):
    groups = {}

    for r in rows:
        groups.setdefault(key(r), []).append(r)

    return groups


def main():
    for name in ("pi_single", "pi_local"):
        if not os.path.exists(os.path.join(BIN, name)):
            sys.exit("Build first: make bin/pi_single bin/pi_local")

    os.makedirs("results", exist_ok=True)

    cores = os.cpu_count() or 1

    # Warm-up run
    run([
        os.path.join(BIN, "pi_local"),
        str(max(1, N // 10)),
        str(cores),
        SEED,
        "0"
    ])

    rows = []

    for r in range(1, RUNS + 1):
        f, v, iv = run([
            os.path.join(BIN, "pi_single"),
            str(N), "1", SEED, str(r)
        ])

        rows.append(f + [v, iv])

        for t in THREADS:
            f, v, iv = run([
                os.path.join(BIN, "pi_local"),
                str(N), str(t), SEED, str(r)
            ])

            rows.append(f + [v, iv])

    with open(
        "results/b_local_scaling.csv", "w", newline=""
    ) as fh:

        w = csv.writer(fh)

        w.writerow([
            "approach", "threads", "run", "time_sec",
            "pi_estimate", "vol_ctxsw", "invol_ctxsw"
        ])

        w.writerows(rows)

    g = summarize(rows, lambda r: (r[0], int(r[1])))

    t_single = statistics.mean(
        float(r[3]) for r in g[("single", 1)]
    )

    print(f"N={N:.0e}, runs={RUNS}, logical CPUs={cores}")
    print(f"single: {t_single:.4f} s\n")

    print(
        "| Threads | Mean time (s) | Std dev (s) | "
        "Speedup | Efficiency | Invol. ctx switches | "
        "pi (run 1) |"
    )

    print("|---|---|---|---|---|---|---|")

    for t in THREADS:
        rs = g[("local", t)]

        times = [float(r[3]) for r in rs]

        m = statistics.mean(times)
        sd = statistics.stdev(times) if len(times) > 1 else 0.0

        sp = t_single / m

        iv = statistics.mean(int(r[6]) for r in rs)

        print(
            f"| {t} | {m:.4f} | {sd:.4f} | "
            f"{sp:.2f}x | {100 * sp / t:.1f}% | "
            f"{iv:.0f} | {rs[0][4]} |"
        )

    # False sharing experiment
    if os.path.exists(FS_BIN):
        fs_rows = []

        for r in range(1, RUNS + 1):
            for t in sorted({1, cores}):
                for mode in ("local", "padded", "unpadded"):
                    f, _, _ = run([
                        FS_BIN, str(N), str(t),
                        SEED, str(r), mode
                    ])

                    fs_rows.append(f)

        with open(
            "results/b_false_sharing.csv", "w", newline=""
        ) as fh:

            w = csv.writer(fh)

            w.writerow([
                "variant", "threads", "run",
                "time_sec", "pi_estimate"
            ])

            w.writerows(fs_rows)

        fg = summarize(
            fs_rows, lambda r: (r[0], int(r[1]))
        )

        print(
            f"\n| Counter placement | 1 thread (s) | "
            f"{cores} threads (s) | Speedup |"
        )

        print("|---|---|---|---|")

        for mode in ("local", "padded", "unpadded"):
            t1 = statistics.mean(
                float(r[3]) for r in fg[(mode, 1)]
            )

            tn = statistics.mean(
                float(r[3]) for r in fg[(mode, cores)]
            )

            print(
                f"| {mode} | {t1:.4f} | "
                f"{tn:.4f} | {t1 / tn:.2f}x |"
            )


if __name__ == "__main__":
    main()

