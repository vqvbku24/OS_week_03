#!/usr/bin/env bash
# scripts/run_tradeoff.sh - Thi nghiem Trade-off dong bo (6.1 & 6.3)
# Nguoi phu trach: Thanh vien E

set -e

TOTAL_POINTS=${TOTAL_POINTS:-100000000}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
TRADEOFF_FILE="results/tradeoff_k.csv"

# Lay so core CPU hien tai cua may (mac dinh 4 neu khong xac dinh duoc)
NUM_CORES=${NUM_CORES:-$(nproc 2>/dev/null || echo 4)}
K_LIST=${K_LIST:-"1 10 100 1000 10000 1000000"}

mkdir -p results

# TODO(E-5): Thi nghiem 6.1: Chay pi_mutex_k voi k thuoc {1,10,100,1000,10000,1000000}, so thread = so core, ghi vao results/tradeoff_k.csv (Thanh vien E)
echo "approach,threads,run,time_sec,pi_estimate" > "$TRADEOFF_FILE"

PROG="bin/pi_mutex_k"
if [ ! -f "$PROG" ] && [ -f "$PROG.exe" ]; then
    PROG="$PROG.exe"
fi

if [ ! -f "$PROG" ]; then
    echo "[run_tradeoff.sh] Loi: Khong tim thay binary $PROG. Vui long chay make truoc."
    exit 1
fi

echo "[run_tradeoff.sh] === BAT DAU THI NGHIEM 6.1 (Tan suat dong bo k) ==="
echo "  - So core su dung: $NUM_CORES"
echo "  - Danh sach k: $K_LIST"
echo "  - So lan lap: $RUNS"

for k in $K_LIST; do
    echo "==> Dang chay voi k = $k"
    for ((r=1; r<=RUNS; r++)); do
        echo "    [Run] k=$k, threads=$NUM_CORES, run=$r/$RUNS"
        ./$PROG $TOTAL_POINTS $NUM_CORES $SEED $r $k >> "$TRADEOFF_FILE"
    done
done

# TODO(E-6): Thi nghiem 6.3: Lay lai so lieu tu results/results.csv (local vs mutex), khong can chay them (Thanh vien E).
echo ""
echo "[run_tradeoff.sh] === THI NGHIEM 6.3 (Local vs Mutex) ==="
echo "  Luu y: Thi nghiem 6.3 se duoc script plot.py tu dong trich xuat tu file results/results.csv."
echo "  Khong can phai chay lai de tiet kiem thoi gian thuc nghiem."
echo ""
echo "[run_tradeoff.sh] Hoan tat! Ket qua thi nghiem 6.1 da ghi vao $TRADEOFF_FILE"
