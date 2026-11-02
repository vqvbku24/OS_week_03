#!/usr/bin/env bash
# scripts/run_bench.sh - Benchmark all approaches across thread configurations
# Assigned Member: Member E

set -e

# TODO(E-3): Parameterize total points, thread list, and run counts via environment variables (Member E)
TOTAL_POINTS=${TOTAL_POINTS:-100000000}
THREADS_LIST=${THREADS_LIST:-"1 2 4 8 16 32 64 100 128"}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
RESULTS_FILE="results/results.csv"
ENV_FILE="report/environment.md"

mkdir -p results report bin

# TODO(E-2): Record lscpu and uname -a outputs into report/environment.md (Member E)
echo "# Experimental Environment" > "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 1. Operating System & Kernel Information (uname -a)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
uname -a 2>/dev/null >> "$ENV_FILE" || echo "uname command unavailable on current environment" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 2. Processor Architecture & Topology (lscpu)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
lscpu 2>/dev/null >> "$ENV_FILE" || echo "lscpu command unavailable (inspect via wmic / systeminfo on Windows)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 3. C Compiler & Toolchain" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
gcc --version | head -n 1 >> "$ENV_FILE" 2>/dev/null || echo "gcc version check failed" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"

# TODO(E-1): Initialize CSV header and append execution output (Member E)
echo "approach,threads,run,time_sec,pi_estimate" > "$RESULTS_FILE"

# TODO(E-4): Perform an unrecorded warm-up run to stabilize CPU frequencies and cache (Member E)
echo "[run_bench.sh] Running warm-up run to stabilize system cache..."
WARMUP_BIN="bin/pi_single"
if [ ! -f "$WARMUP_BIN" ] && [ -f "$WARMUP_BIN.exe" ]; then
    WARMUP_BIN="$WARMUP_BIN.exe"
fi
./$WARMUP_BIN 1000000 1 $SEED 0 > /dev/null 2>&1 || true

echo "[run_bench.sh] Starting formal benchmark suite..."
echo "  - Total points: $TOTAL_POINTS"
echo "  - Thread configurations: $THREADS_LIST"
echo "  - Repetitions per configuration: $RUNS"

APPROACHES="single local mutex atomic"

for app in $APPROACHES; do
    PROG="bin/pi_$app"
    if [ ! -f "$PROG" ] && [ -f "$PROG.exe" ]; then
        PROG="$PROG.exe"
    fi

    if [ ! -f "$PROG" ]; then
        echo "[run_bench.sh] Warning: Binary $PROG not found, please compile first with make."
        continue
    fi

    echo "==> Benchmarking approach: $app"
    for th in $THREADS_LIST; do
        # Single thread approach only executes with thread count 1
        if [ "$app" = "single" ] && [ "$th" -ne 1 ]; then
            continue
        fi

        for ((r=1; r<=RUNS; r++)); do
            echo "    [Run] approach=$app, threads=$th, run=$r/$RUNS"
            ./$PROG $TOTAL_POINTS $th $SEED $r >> "$RESULTS_FILE"
        done
    done
done

echo "[run_bench.sh] Benchmark complete! Results saved to $RESULTS_FILE"
