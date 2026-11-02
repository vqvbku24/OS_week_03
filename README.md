# Lab: Ước Lượng Số π Bằng Phương Pháp Monte Carlo Đa Luồng

Dự án bài tập lớn môn Hệ Điều Hành (Operating Systems) nhằm nghiên cứu, hiện thực và đánh giá hiệu năng các mô hình lập trình đa luồng (multi-threading) thông qua bài toán tính xấp xỉ số $\pi$ bằng phương pháp Monte Carlo.

---

## 1. Phân Công Thành Viên

| Thành viên | Ký hiệu | Vai trò & Trách nhiệm chính | Branch phụ trách |
|---|---|---|---|
| **QVinh** | **A** | Kiến trúc nền tảng (`common.h`), Approach 1 (`pi_single.c`), phương pháp đo | `feat/A-common` |
| Thành viên 2 | **B** | Approach 2 Map-Reduce (`pi_local.c`), Thread scaling & Padding | `feat/B-local` |
| Thành viên 3 | **C** | Approach 3 Mutex Lock (`pi_mutex.c`), Khảo sát Contention | `feat/C-mutex` |
| Thành viên 4 | **D** | Biến thể Batching Mutex (`pi_mutex_k.c`), Atomic (`pi_atomic.c`) | `feat/D-mutexk-atomic` |
| Thành viên 5 | **E** | Kịch bản đo lường tự động (`run_bench.sh`), đồ thị (`plot.py`), tổng hợp báo cáo | `feat/E-bench-plot` |

> Chi tiết từng đầu việc, tiêu chí hoàn thành và danh sách `TODO(X-n)` được quy định chi tiết tại [`TASKS.md`](TASKS.md).

---

## 2. Cấu Trúc Thư Mục

```text
pi-montecarlo-lab/
├── Makefile                # Kịch bản biên dịch dự án
├── README.md               # Hướng dẫn tổng quan
├── TASKS.md                # Bảng phân công chi tiết và checklist từng thành viên
├── .gitignore              # Cấu hình bỏ qua các file build và dữ liệu tạm
├── src/                    # Mã nguồn C (C11, pthread)
│   ├── common.h            # Header dùng chung: parse_args, timer, PRNG, cấu trúc dữ liệu (A)
│   ├── pi_single.c         # Approach 1: Single thread baseline (A)
│   ├── pi_local.c          # Approach 2: Multi-thread Local Accumulation (B)
│   ├── pi_mutex.c          # Approach 3: Shared Counter with Mutex Lock (C)
│   ├── pi_mutex_k.c        # Thí nghiệm 6.1: Mutex batching theo chu kỳ k (D)
│   └── pi_atomic.c         # Multi-thread với chỉ thị phần cứng Atomic Fetch-Add (D)
├── scripts/                # Kịch bản tự động hóa đo lường và vẽ biểu đồ
│   ├── run_bench.sh        # Benchmark tự động toàn diện các cấu hình (E)
│   ├── run_tradeoff.sh     # Thực hiện các thí nghiệm đánh đổi 6.1 & 6.3 (E)
│   └── plot.py             # Xử lý số liệu và vẽ đồ thị (pandas, matplotlib) (E)
├── results/                # Lưu trữ file dữ liệu CSV sau khi đo
├── plots/                  # Lưu trữ các biểu đồ trực quan hóa (.png)
└── report/                 # Báo cáo kết quả
    ├── report.md           # Báo cáo hoàn chỉnh của nhóm
    └── environment.md      # Chi tiết phần cứng và môi trường thực nghiệm
```

---

## 3. Giao Ước Chung (Contract)

### 3.1 Tham số dòng lệnh thống nhất
Mọi chương trình trong thư mục `src/` đều tuân theo chuẩn gọi:
```bash
./prog <total_points> <num_threads> <seed> <run_id> [k]
```
- `total_points`: Tổng số điểm Monte Carlo (mặc định: $10^8 = 100000000$).
- `num_threads`: Số luồng thực thi (ví dụ: $1, 2, 4, 8, 16, \dots$). `pi_single` tự động bỏ qua tham số này.
- `seed`: Giá trị seed gốc để khởi tạo bộ sinh số ngẫu nhiên.
- `run_id`: Định danh lần chạy (ví dụ: $1, 2, 3, \dots$).
- `k`: Tham số chu kỳ đồng bộ (chỉ áp dụng cho `pi_mutex_k`, mặc định $1000$).

### 3.2 Định dạng đầu ra (CSV Output)
Mỗi lần chạy chương trình chỉ in **ĐÚNG MỘT DÒNG DUY NHẤT** ra `stdout` theo định dạng:
```csv
approach,threads,run,time_sec,pi_estimate
```
*Ví dụ:* `local,4,1,0.185240,3.141582000`

---

## 4. Hướng Dẫn Sử Dụng

### 4.1 Biên dịch toàn bộ
```bash
make
# Hoặc trên Windows với MinGW: mingw32-make
```
Tất cả 5 tệp thực thi sẽ được tạo trong thư mục `bin/`.

### 4.2 Kiểm tra nhanh độ chính xác (Smoke Test)
```bash
make check
```
Lệnh này sẽ chạy thử nhanh 5 chương trình với $N = 10^6$ điểm để kiểm tra ước lượng $\pi \approx 3.1416$.

### 4.3 Chạy Benchmark toàn diện
```bash
make bench
```
Chạy script `scripts/run_bench.sh`, tự động lưu số liệu vào `results/results.csv` và cấu hình máy vào `report/environment.md`.

### 4.4 Chạy thí nghiệm Trade-off
```bash
make tradeoff
```
Chạy script `scripts/run_tradeoff.sh` để khảo sát chu kỳ $k$ trong `pi_mutex_k`.

### 4.5 Vẽ biểu đồ
```bash
make plot
```
Đọc dữ liệu từ `results/`, tính trung bình/độ lệch chuẩn/Speedup/Efficiency và xuất các biểu đồ trực quan vào `plots/`.

---

## 5. Quy Ước Phối Hợp Git

1. **Phân nhánh độc lập**: Mỗi thành viên tạo nhánh tương ứng từ `main`:
   - QVinh: `git checkout -b feat/A-common`
   - Thành viên B: `git checkout -b feat/B-local`
   - Thành viên C: `git checkout -b feat/C-mutex`
   - Thành viên D: `git checkout -b feat/D-mutexk-atomic`
   - Thành viên E: `git checkout -b feat/E-bench-plot`
2. **Commit rõ ràng**: Tuân thủ conventional commit (ví dụ: `feat(common): implement xorshift64 PRNG`).
3. **Pull Request & Review**: Mọi thay đổi phải tạo Pull Request vào `main` và có ít nhất 1 thành viên review trước khi merge.
