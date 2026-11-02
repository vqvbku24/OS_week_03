#!/usr/bin/env bash
# scripts/run_tradeoff.sh - Synchronization Trade-off Experiments (6.1 & 6.3)
# Assigned Member: Member E

set -e

TOTAL_POINTS=${TOTAL_POINTS:-100000000}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
TRADEOFF_FILE="results/tradeoff_k.csv"

# Detect number of available CPU cores (default to 4 if detection fails)
NUM_CORES=${NUM_CORES:-$(nproc 2>/dev/null || echo 4)}
K_LIST=${K_LIST:-"1 10 100 1000 10000 1000000"}

mkdir -p results

# TODO(E-5): Experiment 6.1: Run pi_mutex_k with k in {1,10,100,1000,10000,1000000}, threads = cpu cores, output to results/tradeoff_k.csv (Member E)
echo "approach,threads,run,time_sec,pi_estimate" > "$TRADEOFF_FILE"

PROG="bin/pi_mutex_k"
if [ ! -f "$PROG" ] && [ -f "$PROG.exe" ]; then
    PROG="$PROG.exe"
fi

if [ ! -f "$PROG" ]; then
    echo "[run_tradeoff.sh] Error: Binary $PROG not found. Please run make first."
    exit 1
fi

echo "[run_tradeoff.sh] === STARTING EXPERIMENT 6.1 (Mutex Synchronization Frequency k) ==="
echo "  - CPU core count: $NUM_CORES"
echo "  - List of k values: $K_LIST"
echo "  - Repetitions per k: $RUNS"

for k in $K_LIST; do
    echo "==> Running with k = $k"
    for ((r=1; r<=RUNS; r++)); do
        echo "    [Run] k=$k, threads=$NUM_CORES, run=$r/$RUNS"
        ./$PROG $TOTAL_POINTS $NUM_CORES $SEED $r $k >> "$TRADEOFF_FILE"
    done
done

# TODO(E-6): Experiment 6.3: Reuse data from results/results.csv (local vs mutex), avoiding redundant runs (Member E)
echo ""
echo "[run_tradeoff.sh] === EXPERIMENT 6.3 (Local Accumulation vs Mutex) ==="
echo "  Note: Experiment 6.3 data is automatically extracted from results/results.csv by plot.py."
echo "  No redundant execution needed."
echo ""
echo "[run_tradeoff.sh] Finished! Experiment 6.1 results written to $TRADEOFF_FILE"
