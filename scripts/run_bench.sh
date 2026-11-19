#!/usr/bin/env bash
# scripts/run_bench.sh - Run Monte Carlo pi benchmarks across approaches and threads
# Assigned Member: Member E

set -e

TOTAL_POINTS=${TOTAL_POINTS:-100000000}
THREADS_LIST=${THREADS_LIST:-"1 2 4 8 16 32 64 100 128"}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
RESULTS_FILE="results/results.csv"
ENV_FILE="report/environment.md"

mkdir -p results report

# Append basic system info without overwriting existing details
if [ ! -s "$ENV_FILE" ]; then
    echo "# Experimental Environment" > "$ENV_FILE"
fi

if ! grep -q "## System Command Output" "$ENV_FILE" 2>/dev/null; then
    cat >> "$ENV_FILE" <<EOF

## System Command Output
\`\`\`
$(uname -a 2>/dev/null || echo "uname unavailable")
\`\`\`

\`\`\`
$(lscpu 2>/dev/null || echo "lscpu unavailable (CPU logical cores: $(nproc 2>/dev/null || echo unknown))")
\`\`\`

- **Compiler**: $(gcc --version 2>/dev/null | head -n 1 || echo "gcc")
- **Flags**: \`-O2 -std=c11 -Wall -Wextra -pthread\`
EOF
fi

# Write CSV header
echo "approach,threads,run,time_sec,pi_estimate" > "$RESULTS_FILE"

# One unrecorded warm-up run
WARMUP_BIN="bin/pi_single"
[ -f "$WARMUP_BIN.exe" ] && WARMUP_BIN="$WARMUP_BIN.exe"
./"$WARMUP_BIN" 1000000 1 "$SEED" 0 > /dev/null 2>&1 || true

# Run benchmarks
for app in single local mutex atomic; do
    PROG="bin/pi_$app"
    [ -f "$PROG.exe" ] && PROG="$PROG.exe"

    echo "Benchmarking $app..."
    for th in $THREADS_LIST; do
        if [ "$app" = "single" ] && [ "$th" -ne 1 ]; then
            continue
        fi

        for ((r=1; r<=RUNS; r++)); do
            echo "  approach=$app threads=$th run=$r/$RUNS"
            ./"$PROG" "$TOTAL_POINTS" "$th" "$SEED" "$r" >> "$RESULTS_FILE"
        done
    done
done

echo "Done! Results saved to $RESULTS_FILE"
