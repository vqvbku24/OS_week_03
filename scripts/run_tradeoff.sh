#!/usr/bin/env bash
# scripts/run_tradeoff.sh - Trade-off experiments 6.1 (batch size k) and 6.3 (local vs mutex)
# Assigned Member: Member E

set -e

TOTAL_POINTS=${TOTAL_POINTS:-100000000}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
NUM_CORES=${NUM_CORES:-$(nproc 2>/dev/null || echo 4)}
K_LIST=${K_LIST:-"1 10 100 1000 10000 1000000"}
TRADEOFF_FILE="results/tradeoff_k.csv"

mkdir -p results

echo "approach,threads,run,time_sec,pi_estimate" > "$TRADEOFF_FILE"

PROG="bin/pi_mutex_k"
[ -f "$PROG.exe" ] && PROG="$PROG.exe"

echo "Running Experiment 6.1 (threads=$NUM_CORES)..."
for k in $K_LIST; do
    echo "  k=$k"
    for ((r=1; r<=RUNS; r++)); do
        ./"$PROG" "$TOTAL_POINTS" "$NUM_CORES" "$SEED" "$r" "$k" >> "$TRADEOFF_FILE"
    done
done

echo "Experiment 6.3 (local vs mutex) reuses results/results.csv in scripts/plot.py."
echo "Done! Results saved to $TRADEOFF_FILE"
