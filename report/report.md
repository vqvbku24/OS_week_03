# Báo Cáo Bài Lab: Ước Lượng Số π Bằng Phương Pháp Monte Carlo Với Đa Luồng

**Nhóm thực hiện**: 5 thành viên
- **Thành viên A**: QVinh (Nền tảng, `common.h`, `pi_single.c`)
- **Thành viên B**: (Approach 2: `pi_local.c`, phân tích scaling)
- **Thành viên C**: (Approach 3: `pi_mutex.c`, phân tích contention)
- **Thành viên D**: (Biến thể `pi_mutex_k.c`, `pi_atomic.c`, trade-off k)
- **Thành viên E**: (Benchmark, đồ thị, tổng hợp báo cáo)

---

## 1. Giới thiệu và Môi trường thử nghiệm
*Người phụ trách: Thành viên E*

<!-- TODO(E-12): Viết phần giới thiệu bài toán Monte Carlo tính số pi và tóm tắt cấu hình thực nghiệm từ report/environment.md (Thành viên E) -->

### 1.1 Bài toán tính π bằng Monte Carlo
- Nguyên lý: Sinh ngẫu nhiên $N$ điểm đồng đều $(x, y) \in [0, 1] \times [0, 1]$.
- Điều kiện điểm nằm trong 1/4 hình tròn đơn vị: $x^2 + y^2 \le 1.0$.
- Tỉ số diện tích: $\frac{\text{Diện tích 1/4 hình tròn}}{\text{Diện tích hình vuông đơn vị}} = \frac{\frac{\pi \times 1^2}{4}}{1 \times 1} = \frac{\pi}{4}$.
- Công thức ước lượng: $\pi \approx 4 \times \frac{\text{Số điểm rơi vào hình tròn}}{\text{Tổng số điểm sinh ra}} = 4 \times \frac{M}{N}$.

### 1.2 Môi trường thực nghiệm
- Chi tiết xem file [`environment.md`](environment.md).

---

## 2. Thiết kế các Phương Pháp (Approaches)

### 2.1 Approach 1: Single Thread (Baseline)
*Người phụ trách: QVinh (Thành viên A)*

<!-- TODO(A-8): Trình bày thiết kế Single Thread: vòng lặp sinh số, RNG xorshift64, không có đa luồng, làm mốc baseline đo lường (QVinh - Thành viên A) -->

### 2.2 Approach 2: Multi-thread Local Accumulation (Map-Reduce)
*Người phụ trách: Thành viên B*

<!-- TODO(B-6): Trình bày thiết kế Map-Reduce: chia việc cho các luong, biến cục bộ local_inside, mảng kết quả dùng padding padded_u64 triệt tiêu False Sharing, giai đoạn Reduce (Thành viên B) -->

### 2.3 Approach 3: Shared Counter with Mutex Lock
*Người phụ trách: Thành viên C*

<!-- TODO(C-4): Trình bày thiết kế Shared Counter: một biến shared_inside toàn cục, pthread_mutex_lock/unlock bao quanh mỗi lần tăng biến, nguyên nhân gây thắt nút cổ chai (Thành viên C) -->

### 2.4 Biến thể Mutex theo chu kỳ k và Atomic
*Người phụ trách: Thành viên D*

<!-- TODO(D-6): Trình bày thiết kế biến thể: gom nhóm cập nhật theo batch k điểm (pi_mutex_k) và sử dụng chỉ thị phần cứng không khóa __atomic_fetch_add (pi_atomic) (Thành viên D) -->

---

## 3. Phương Pháp Đo Lường & Đảm Bảo Tính Nhất Quán
*Người phụ trách: QVinh (Thành viên A)*

<!-- TODO(A-9): Mô tả chi tiết phương pháp đo:
  - Sử dụng clock_gettime(CLOCK_MONOTONIC) để đo chính xác phần tính toán (không đo parse args, cấp phát, in CSV).
  - Cố định seed gốc và công thức sinh seed độc lập cho từng luồng.
  - Số lần lặp lại (>= 5 lần) để loại bỏ nhiễu và tính độ lệch chuẩn.
  - Loại bỏ hoàn toàn rand()/srand() do tranh chấp trạng thái toàn cục. (QVinh - Thành viên A) -->

---

## 4. Kết Quả Thực Nghiệm
*Người phụ trách: Thành viên E*

<!-- TODO(E-13): Đưa bảng số liệu tổng hợp (thời gian trung bình, độ lệch chuẩn, speedup, efficiency) và chèn các biểu đồ từ thư mục plots/ (Thành viên E) -->

### 4.1 Bảng số liệu tổng hợp
*(Được trích xuất từ `results/results.csv` sau khi chạy benchmark)*

| Approach | Threads | Thời gian trung bình (s) | Độ lệch chuẩn (s) | Speedup | Parallel Efficiency |
|---|---|---|---|---|---|
| single | 1 | ... | ... | 1.00x | 100% |
| local | 2 | ... | ... | ... | ... |
| local | 4 | ... | ... | ... | ... |
| mutex | 2 | ... | ... | ... | ... |
| atomic | 2 | ... | ... | ... | ... |

### 4.2 Các biểu đồ thực nghiệm
- **Biểu đồ thời gian thực thi theo số luồng**: `plots/time_vs_threads.png`
- **Biểu đồ Speedup theo số luồng**: `plots/speedup_vs_threads.png`
- **Biểu đồ Parallel Efficiency**: `plots/efficiency_vs_threads.png`
- **Biểu đồ Trade-off tần suất đồng bộ k**: `plots/tradeoff_k.png`
- **Biểu đồ So sánh Local vs Mutex**: `plots/comparison_local_vs_mutex.png`

---

## 5. Phân Tích Chuyên Sâu

### 5.2.A Vì sao Speedup không tuyến tính?
*Người phụ trách: QVinh (Thành viên A)*

<!-- TODO(A-10): Phân tích định luật Amdahl, chi phí tạo và hủy pthread (overhead), giới hạn băng thông bộ nhớ và hiện tượng diminishing returns khi số luồng tăng (QVinh - Thành viên A) -->

### 5.2.B Điểm bão hòa (Saturation Point)
*Người phụ trách: Thành viên B*

<!-- TODO(B-7): Phân tích điểm bão hòa của hiệu năng, mối liên hệ trực tiếp với số nhân vật lý/logic của CPU máy thử nghiệm (Thành viên B) -->

### 5.2.C Hiện tượng Oversubscription & Chi phí Context Switch
*Người phụ trách: Thành viên B*

<!-- TODO(B-8): Phân tích hiện tượng số luồng vượt quá số CPU core (vd: 32, 64, 128 luồng trên máy 4-8 core): chi phí context switch, tranh chấp scheduler và cache thrashing (Thành viên B) -->

### 5.2.D Chi phí Đồng Bộ & Serialization (Mutex Contention)
*Người phụ trách: Thành viên C*

<!-- TODO(C-5): Phân tích nguyên nhân Approach 3 (Mutex) chạy chậm hơn hàng trăm lần so với Approach 2: hiện tượng serialization luồng, hàng đợi chờ lock, suy thoái từ đa luồng thành đơn luồng bị phạt nặng (Thành viên C) -->

### 5.2.E Sự suy giảm Hiệu Suất Song Song (Parallel Efficiency)
*Người phụ trách: Thành viên E*

<!-- TODO(E-14): Phân tích công thức Efficiency = Speedup / n, giải thích vì sao khi số luồng tăng thì efficiency giảm dần theo quy luật kinh tế lợi tức giảm dần (Diminishing returns) (Thành viên E) -->

---

## 6. Đánh Giá Trade-Off (Đánh Đổi Hiệu Năng)

### 6.1 Trade-Off Tần Suất Đồng Bộ (Chu kỳ $k$)
*Người phụ trách: Thành viên D*

<!-- TODO(D-7): Phân tích kết quả thí nghiệm 6.1 từ results/tradeoff_k.csv:
  - Khi k = 1: Chi phí đồng bộ cao nhất, tương đương mutex thông thường.
  - Khi k tăng (10 -> 1000 -> 1000000): Tần suất lock giảm, thời gian tiệm cận dần Approach 2 (Local).
  - Đưa ra khuyến nghị giá trị k tối ưu để cân bằng giữa độ trễ cập nhật và hiệu năng. (Thành viên D) -->

### 6.2 Phân tích Thread Scaling
*Người phụ trách: Thành viên B*

<!-- TODO(B-9): Đánh giá 3 giai đoạn của đồ thị Scaling trong Approach 2:
  - Giai đoạn 1: Threads < Cores (Tăng tốc mạnh).
  - Giai đoạn 2: Threads ≈ Cores (Bình nguyên - Plateau).
  - Giai đoạn 3: Threads > Cores (Bão hòa hoặc suy giảm do Oversubscription). (Thành viên B) -->

### 6.3 Local Accumulation vs Shared Counter
*Người phụ trách: Thành viên C*

<!-- TODO(C-6): So sánh đối đầu giữa kiến trúc chia sẻ (Shared state) và kiến trúc không chia sẻ (Share-nothing / Local state) dựa trên dữ liệu đo lường thực tế (Thành viên C) -->

---

## 7. Best Practices & Kết Luận
*Người phụ trách: Toàn nhóm (Thành viên E tổng hợp)*

<!-- TODO(E-15): Tổng hợp các nguyên tắc thiết kế lập trình đa luồng rút ra từ bài lab (Best Practices) và kết luận bài toán (Thành viên E tổng hợp) -->
- **Tránh chia sẻ biến trong vòng lặp tính toán**: Luôn ưu tiên Local Accumulation và cấu trúc Map-Reduce.
- **Triệt tiêu False Sharing**: Chú ý kích thước Cache Line (64 bytes) và cấu trúc padding.
- **Lựa chọn cơ chế đồng bộ phù hợp**:
  - Không khóa (Lock-free) > Gom nhóm (Batching) > Khóa cục bộ > Khóa liên tục.
- **RNG độc lập**: Tuyệt đối không dùng các hàm có trạng thái chia sẻ toàn cục như `rand()`, mỗi thread phải có PRNG riêng biệt (`xorshift64`).
- **Xác định ngưỡng luồng tối ưu**: Giới hạn số worker thread không vượt quá số core CPU vật lý để tránh oversubscription.
