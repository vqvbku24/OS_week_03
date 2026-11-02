#!/usr/bin/env bash
# scripts/run_bench.sh - Benchmark tat ca cac approach
# Nguoi phu trach: Thanh vien E

set -e

# TODO(E-3): Tham so hoa N, danh sach thread, so lan chay bang bien moi truong (Thanh vien E)
TOTAL_POINTS=${TOTAL_POINTS:-100000000}
THREADS_LIST=${THREADS_LIST:-"1 2 4 8 16 32 64 100 128"}
RUNS=${RUNS:-5}
SEED=${SEED:-12345}
RESULTS_FILE="results/results.csv"
ENV_FILE="report/environment.md"

mkdir -p results report bin

# TODO(E-2): Ghi thong tin lscpu va uname -a vao report/environment.md (Thanh vien E)
echo "# Môi Trường Thực Nghiệm (Environment)" > "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 1. Thông tin hệ thống (uname -a)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
uname -a 2>/dev/null >> "$ENV_FILE" || echo "uname khong kha dung tren moi truong hien tai" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 2. Thông tin CPU (lscpu)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
lscpu 2>/dev/null >> "$ENV_FILE" || echo "lscpu khong kha dung (kiem tra qua wmic / systeminfo tren Windows)" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
echo "" >> "$ENV_FILE"
echo "## 3. Trình biên dịch" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"
gcc --version | head -n 1 >> "$ENV_FILE" 2>/dev/null || echo "gcc version" >> "$ENV_FILE"
echo '```' >> "$ENV_FILE"

# TODO(E-1): Khoi tao header cho results.csv va noi output cua tung lan chay (Thanh vien E)
echo "approach,threads,run,time_sec,pi_estimate" > "$RESULTS_FILE"

# TODO(E-4): Them mot lan chay warm-up khong ghi ket qua de CPU/cache on dinh (Thanh vien E)
echo "[run_bench.sh] Chay warm-up khoi dong he thong..."
WARMUP_BIN="bin/pi_single"
if [ ! -f "$WARMUP_BIN" ] && [ -f "$WARMUP_BIN.exe" ]; then
    WARMUP_BIN="$WARMUP_BIN.exe"
fi
./$WARMUP_BIN 1000000 1 $SEED 0 > /dev/null 2>&1 || true

echo "[run_bench.sh] Bat dau chay benchmark chinh thuc..."
echo "  - Total points: $TOTAL_POINTS"
echo "  - Danh sach threads: $THREADS_LIST"
echo "  - So lan lap moi cau hinh: $RUNS"

APPROACHES="single local mutex atomic"

for app in $APPROACHES; do
    PROG="bin/pi_$app"
    if [ ! -f "$PROG" ] && [ -f "$PROG.exe" ]; then
        PROG="$PROG.exe"
    fi

    if [ ! -f "$PROG" ]; then
        echo "[run_bench.sh] Canh bao: Khong tim thay binary $PROG, vui long make truoc."
        continue
    fi

    echo "==> Dang chay approach: $app"
    for th in $THREADS_LIST; do
        # Single thread chi chay voi 1 thread
        if [ "$app" = "single" ] && [ "$th" -ne 1 ]; then
            continue
        fi

        for ((r=1; r<=RUNS; r++)); do
            echo "    [Run] approach=$app, threads=$th, run=$r/$RUNS"
            ./$PROG $TOTAL_POINTS $th $SEED $r >> "$RESULTS_FILE"
        done
    done
done

echo "[run_bench.sh] Hoan tat benchmark! Ket qua da duoc ghi vao $RESULTS_FILE"
