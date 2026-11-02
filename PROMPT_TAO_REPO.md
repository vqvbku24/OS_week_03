# PROMPT: Tạo repo Lab Monte Carlo π (nhóm 5 người)

> Cách dùng: mở thư mục trống, chạy Claude Code (hoặc dán vào công cụ AI bất kỳ có quyền tạo file), rồi dán toàn bộ phần trong khung bên dưới.

---

````
Bạn là trợ lý dựng khung dự án. Hãy tạo từ đầu một repo Git tên `pi-montecarlo-lab` cho bài lab
"Tính π bằng Monte Carlo với đa luồng" của một nhóm 5 thành viên (A, B, C, D, E).
Mục tiêu: repo phải BIÊN DỊCH ĐƯỢC NGAY (`make` chạy thành công) dù nhiều chỗ còn TODO,
để mỗi thành viên mở file của mình và điền phần còn thiếu mà không làm hỏng phần người khác.

## 1. Yêu cầu bài lab (tóm tắt)
- Approach 1: single thread (baseline).
- Approach 2: multi-thread, local accumulation (map-reduce), KHÔNG có biến chia sẻ trong vòng lặp chính,
  mỗi thread có RNG/seed riêng.
- Approach 3: shared counter + mutex, cập nhật mỗi lần điểm rơi vào hình tròn.
- Thí nghiệm trade-off: 6.1 (tần suất đồng bộ, cập nhật mỗi k vòng), 6.2 (thread scaling),
  6.3 (local vs shared). Làm cả ba.
- Đo: ≥ 5 lần/cấu hình, lấy trung bình, N cố định, chỉ đo phần tính toán (không đo init/in ấn).
- Phân tích: A speedup phi tuyến, B điểm bão hòa, C oversubscription, D chi phí đồng bộ, E parallel efficiency.
- Deliverables: source, số liệu, đồ thị (time, speedup, efficiency), phân tích, best practices.

## 2. Quy ước chung (HỢP ĐỒNG, mọi file phải tuân theo)
- Ngôn ngữ: C (C11), pthread. Biên dịch: `gcc -O2 -pthread -std=c11 -Wall -Wextra`.
- Tham số dòng lệnh thống nhất: `./prog <total_points> <num_threads> <seed> <run_id> [k]`
  (`pi_single` bỏ qua num_threads; chỉ `pi_mutex_k` dùng k).
- Mỗi lần chạy in ĐÚNG MỘT dòng CSV ra stdout, không in gì khác:
  `approach,threads,run,time_sec,pi_estimate`
  Với `pi_mutex_k`, dùng tên approach dạng `mutex_k<k>` (ví dụ `mutex_k1000`).
- Tên approach cố định: `single`, `local`, `mutex`, `mutex_k<k>`, `atomic`.
- Mặc định: N = 100000000 (1e8), seed gốc 12345, thread i dùng seed = base_seed + i.
- Dãy thread mặc định: 1 2 4 8 16 32 64 100 128 (cấu hình được trong script).
- Chỉ đo thời gian từ lúc bắt đầu tạo thread đến khi join và reduce xong, bằng clock_gettime(CLOCK_MONOTONIC).
- Không dùng rand()/srand() (trạng thái toàn cục). Dùng xorshift64 riêng mỗi thread.
- Chia việc: thread i nhận total/n điểm, thread cuối nhận thêm phần dư.

## 3. Cấu trúc repo cần tạo
```
pi-montecarlo-lab/
├── README.md
├── TASKS.md
├── Makefile
├── .gitignore
├── src/
│   ├── common.h        (Thành viên A)
│   ├── pi_single.c     (A)
│   ├── pi_local.c      (B)
│   ├── pi_mutex.c      (C)
│   ├── pi_mutex_k.c    (D)
│   └── pi_atomic.c     (D)
├── scripts/
│   ├── run_bench.sh      (E)
│   ├── run_tradeoff.sh   (E)
│   └── plot.py           (E)
├── results/.gitkeep
├── plots/.gitkeep
└── report/
    ├── report.md
    └── environment.md
```

## 4. Quy tắc đánh dấu TODO
- Mọi chỗ cần người làm phải có comment dạng: `TODO(<Người>-<số>): <việc cụ thể>`
  ví dụ `// TODO(B-2): khởi tạo xorshift với seed = base_seed + thread_id`.
- Mỗi TODO phải nêu rõ: việc cần làm, gợi ý ngắn, và tiêu chí hoàn thành.
- Trong code Python/bash/Markdown dùng cùng cú pháp TODO(<Người>-<số>).
- Phần BẠN tự viết hoàn chỉnh (không để TODO): Makefile, .gitignore, README.md, khung `main()`
  (parse tham số, in CSV) của mỗi chương trình, và phần khai báo hàm trong common.h.
- Phần để TODO cho thành viên: thân hàm cốt lõi, vòng lặp Monte Carlo, logic thread, mutex, atomic,
  script chạy benchmark, vẽ đồ thị, nội dung báo cáo.
- Các stub phải biên dịch được và chạy không crash (ví dụ trả về 0 hoặc π giả = 0.0),
  kèm `fprintf(stderr, "STUB: ...")` để dễ nhận biết chưa làm xong.

## 5. Chi tiết từng file

### Makefile (viết hoàn chỉnh)
- Target: `all` (build 5 binary vào `bin/`), `clean`, `bench` (gọi scripts/run_bench.sh),
  `tradeoff` (gọi scripts/run_tradeoff.sh), `plot` (gọi python3 scripts/plot.py), `check` (chạy nhanh từng chương trình với N=1e6, 2 thread để kiểm tra π ≈ 3.14).
- Cờ: `CFLAGS = -O2 -std=c11 -Wall -Wextra -pthread`.

### src/common.h (Thành viên A)
Khai báo (inline hoặc static inline) các hàm sau, mỗi hàm có TODO(A-x) cho phần thân:
- `double now_sec(void);`  TODO(A-1): dùng clock_gettime(CLOCK_MONOTONIC).
- `uint64_t xorshift64(uint64_t *state);`  TODO(A-2): xorshift64 chuẩn (13, 7, 17); chú ý state ≠ 0.
- `double rand01(uint64_t *state);`  TODO(A-3): chuyển sang double trong [0,1) bằng 53 bit cao.
- `uint64_t split_work(uint64_t total, int nthreads, int i);`  TODO(A-4): thread cuối nhận phần dư.
- `uint64_t seed_for_thread(uint64_t base, int i);`  TODO(A-5): trả base + i, đảm bảo khác 0 và trộn thêm (splitmix64) nếu cần.
- `void print_csv(const char *approach, int threads, int run, double time_sec, double pi);`
  Viết hoàn chỉnh: in đúng định dạng CSV một dòng.
- `typedef struct { uint64_t total; int threads; uint64_t seed; int run; uint64_t k; } config_t;`
  và `config_t parse_args(int argc, char **argv);` viết hoàn chỉnh, báo lỗi rõ ràng khi thiếu tham số.
- Macro `CACHE_LINE 64` và ví dụ cấu trúc padding `typedef struct { uint64_t value; char pad[CACHE_LINE-8]; } padded_u64;`

### src/pi_single.c (A)
- Khung main hoàn chỉnh (parse, đo thời gian, print_csv).
- TODO(A-6): vòng lặp sinh total điểm (x,y) trong [-1,1] (hoặc [0,1] với x²+y²≤1), đếm số điểm trong hình tròn.
- TODO(A-7): tính π = 4 * inside / total, kiểm tra ≈ 3.1416.

### src/pi_local.c (B)
- Khung: tạo mảng thread, truyền struct tham số (id, count, seed) cho mỗi thread, join, reduce.
- TODO(B-1): hàm worker dùng BIẾN LOCAL `local_inside`, chỉ ghi vào `results[id]` (kiểu padded_u64) khi kết thúc.
- TODO(B-2): khởi tạo RNG riêng mỗi thread bằng seed_for_thread.
- TODO(B-3): chia việc bằng split_work.
- TODO(B-4): reduce sau khi join, tính π.
- TODO(B-5): giải thích trong comment vì sao dùng padding (false sharing).

### src/pi_mutex.c (C)
- Khung giống pi_local.c nhưng có biến global `shared_inside` và `pthread_mutex_t lock`.
- TODO(C-1): worker lock/unlock QUANH MỖI LẦN tăng biến chia sẻ, bên trong vòng lặp (để thấy contention).
- TODO(C-2): khởi tạo/hủy mutex đúng cách.
- TODO(C-3): xác nhận kết quả đúng (không race) bằng cách so với pi_local ở cùng seed.

### src/pi_mutex_k.c (D)
- Tham số thêm `k`.
- TODO(D-1): mỗi thread đếm local, cứ k điểm hoặc khi kết thúc thì lock → cộng vào global → unlock → reset local.
- TODO(D-2): xử lý đúng phần dư cuối cùng (không mất điểm khi total không chia hết k).
- TODO(D-3): kiểm tra: k=1 hành vi giống pi_mutex, k rất lớn hành vi giống pi_local.

### src/pi_atomic.c (D)
- TODO(D-4): dùng `__atomic_fetch_add(&shared, 1, __ATOMIC_RELAXED)` thay mutex, cập nhật mỗi lần điểm rơi trong.
- TODO(D-5): ghi chú so sánh mutex vs atomic vs local trong comment đầu file.

### scripts/run_bench.sh (E)
- Vòng lặp: approach ∈ {single, local, mutex, atomic} × threads ∈ danh sách × 5 lần chạy.
- TODO(E-1): chạy từng cấu hình, nối stdout vào `results/results.csv` (có header `approach,threads,run,time_sec,pi_estimate`).
- TODO(E-2): ghi `lscpu` và `uname -a` vào `report/environment.md`.
- TODO(E-3): tham số hóa N, danh sách thread, số lần chạy bằng biến môi trường.
- TODO(E-4): thêm một lần chạy "warm-up" không ghi kết quả.

### scripts/run_tradeoff.sh (E)
- TODO(E-5): thí nghiệm 6.1: chạy pi_mutex_k với k ∈ {1,10,100,1000,10000,1000000}, số thread = số core, ghi `results/tradeoff_k.csv`.
- TODO(E-6): thí nghiệm 6.3 lấy lại từ results.csv (local vs mutex), không cần chạy thêm.

### scripts/plot.py (E)
- Dùng pandas + matplotlib.
- TODO(E-7): đọc results.csv, tính trung bình và độ lệch chuẩn theo (approach, threads).
- TODO(E-8): speedup = T_single / T_parallel(n); efficiency = speedup / n.
- TODO(E-9): vẽ 3 đồ thị cho mỗi approach (time, speedup, efficiency) + 1 đồ thị so sánh chung; lưu PNG vào `plots/`.
- TODO(E-10): đồ thị 6.1: thời gian theo k (trục log).
- TODO(E-11): đánh dấu đường thẳng đứng tại số core của máy.

### report/report.md
Tạo template với các mục sau, mỗi mục ghi rõ người phụ trách và có `TODO(<Người>-<số>)`:
1. Giới thiệu và môi trường (E)
2. Thiết kế 3 approach (A: Approach 1; B: Approach 2; C: Approach 3; D: biến thể k và atomic)
3. Phương pháp đo (A)
4. Kết quả (bảng + đồ thị) (E)
5. Phân tích: A→mục 5.2.A (A), B→5.2.B (B), C→5.2.C (B), D→5.2.D (C), E→5.2.E (E)
6. Trade-off: 6.1 (D), 6.2 (B), 6.3 (C)
7. Best practices và kết luận (cả nhóm, E tổng hợp)

### README.md (viết hoàn chỉnh)
Giới thiệu bài lab, cấu trúc repo, cách build (`make`), chạy thử (`make check`), chạy benchmark (`make bench`),
vẽ đồ thị (`make plot`), hợp đồng CSV/tham số, quy ước Git (mỗi người một branch: `feat/A-common`, `feat/B-local`,
`feat/C-mutex`, `feat/D-mutexk-atomic`, `feat/E-bench-plot`; PR vào `main`, cần ít nhất 1 người review).

### TASKS.md (viết hoàn chỉnh, quan trọng)
Tổng hợp việc cho từng thành viên. Với MỖI thành viên A–E, tạo một mục gồm:
- Vai trò (một dòng).
- Danh sách file phụ trách.
- Checklist `- [ ]` liệt kê TẤT CẢ TODO(<Người>-<số>) của người đó (trích tự động từ code, ghi kèm file và mô tả ngắn).
- Phần phân tích/báo cáo phải viết.
- Phụ thuộc: cần ai xong gì trước.
- Tiêu chí hoàn thành (definition of done).
Thêm cuối file: bảng timeline 7 ngày, danh sách review chéo (A xem B, B xem C, C xem D, D xem E, E xem A),
và checklist nộp bài (source, số liệu, đồ thị, phân tích, best practices).

## 6. Sau khi tạo xong
1. Chạy `make` và xác nhận biên dịch thành công, báo lại số cảnh báo (nếu có).
2. Chạy `grep -rn "TODO(" . | sort` và in ra danh sách để đối chiếu với TASKS.md (không được thiếu TODO nào).
3. `git init`, tạo commit đầu tiên "chore: scaffold repo", tạo sẵn 5 branch theo README.
4. In ra cây thư mục cuối cùng và hướng dẫn 3 dòng cho thành viên mới bắt đầu.
Không tự điền các TODO. Nếu có điểm mơ hồ, chọn phương án đơn giản nhất và ghi chú trong README.
````

---

## Lưu ý khi dùng prompt này

- Nếu muốn dùng C++ (`std::thread`) thay C, đổi mục "Quy ước chung" và đuôi file sang `.cpp` trước khi dán.
- Sau khi AI tạo xong, nhờ nó chạy bước 6.2 để kiểm tra danh sách TODO khớp với `TASKS.md`.
