# TASKS: Phân Công Công Việc Nhóm (Lab Monte Carlo π)

Quy ước chung: `TODO(X-n)` là mã định danh công việc tương ứng trong mã nguồn và báo cáo. Thành viên tick `[x]` khi hoàn thành và đã kiểm tra (merge vào `main`).

---

## Thành viên A: QVinh — Nền Tảng Chung & Approach 1 (Single Thread)

**Vai trò**: Trưởng nhóm / Kiến trúc sư nền tảng. Cung cấp file giao ước chung `common.h` và bản đơn luồng làm baseline. **Cần hoàn thành sớm nhất (Ngày 1–2).**

**File phụ trách**: `src/common.h`, `src/pi_single.c`, các mục tương ứng trong `report/report.md`.

**Checklist công việc**
- [ ] TODO(A-1) `src/common.h`: Hàm `now_sec()` sử dụng `clock_gettime(CLOCK_MONOTONIC)`.
- [ ] TODO(A-2) `src/common.h`: Hàm sinh số ngẫu nhiên `xorshift64()` chuẩn (13, 7, 17), kiểm tra `state != 0`.
- [ ] TODO(A-3) `src/common.h`: Hàm `rand01()` chuyển đổi sang số `double` trong nửa khoảng `[0, 1)`.
- [ ] TODO(A-4) `src/common.h`: Hàm `split_work()`, phân chia đều việc và cộng phần dư cho luồng cuối cùng.
- [ ] TODO(A-5) `src/common.h`: Hàm `seed_for_thread()`, tạo seed riêng biệt độc lập cho từng luồng.
- [ ] TODO(A-6) `src/pi_single.c`: Vòng lặp Monte Carlo sinh điểm và đếm số điểm rơi vào hình tròn.
- [ ] TODO(A-7) `src/pi_single.c`: Tính $\pi = 4.0 \times \frac{\text{inside}}{\text{total}}$, kiểm tra kết quả $\approx 3.1416$.
- [ ] TODO(A-8) `report/report.md`: Viết Mục 2.1 — Thiết kế Approach 1 (Single thread baseline).
- [ ] TODO(A-9) `report/report.md`: Viết Mục 3 — Phương pháp đo lường (clock monotonic, seed độc lập, $\ge 5$ lần chạy).
- [ ] TODO(A-10) `report/report.md`: Viết Mục 5.2.A — Phân tích nguyên nhân tốc độ tăng tốc phi tuyến (Định luật Amdahl, thread overhead, diminishing returns).

**Phụ thuộc**: Không phụ thuộc thành viên khác.
**Định nghĩa hoàn thành (DoD)**:
- Chạy `./bin/pi_single 1000000 1 12345 1` cho kết quả $\pi \approx 3.1416$.
- `common.h` hoàn chỉnh để các thành viên B, C, D biên dịch và phát triển tiếp.

---

## Thành viên B: Approach 2 (Local Accumulation / Map-Reduce)

**Vai trò**: Hiện thực phiên bản song song tối ưu không chia sẻ trạng thái, đánh giá thread scaling.

**File phụ trách**: `src/pi_local.c`, các mục tương ứng trong `report/report.md`.

**Checklist công việc**
- [ ] TODO(B-1) `src/pi_local.c`: Worker dùng biến cục bộ `local_inside`, chỉ ghi vào `results[id]` khi kết thúc.
- [ ] TODO(B-2) `src/pi_local.c`: Khởi tạo PRNG riêng biệt cho mỗi luồng thông qua `seed_for_thread`.
- [ ] TODO(B-3) `src/pi_local.c`: Chia đều khối lượng tính toán bằng `split_work` và gọi `pthread_create`.
- [ ] TODO(B-4) `src/pi_local.c`: Thu gom luồng (`pthread_join`), thực hiện Reduce tổng và tính $\pi$.
- [ ] TODO(B-5) `src/pi_local.c`: Viết giải thích trong mã nguồn về kỹ thuật padding `padded_u64` chống False Sharing.
- [ ] TODO(B-6) `report/report.md`: Viết Mục 2.2 — Thiết kế Approach 2 (Map phase, Reduce phase, memory layout).
- [ ] TODO(B-7) `report/report.md`: Viết Mục 5.2.B — Phân tích điểm bão hòa (Saturation point) và mối liên hệ với số core CPU.
- [ ] TODO(B-8) `report/report.md`: Viết Mục 5.2.C — Hiện tượng quá tải luồng (Oversubscription) và chi phí context switch.
- [ ] TODO(B-9) `report/report.md`: Viết Mục 6.2 — Phân tích chi tiết thí nghiệm Thread Scaling qua 3 giai đoạn.

**Phụ thuộc**: `src/common.h` của QVinh (Thành viên A).
**Định nghĩa hoàn thành (DoD)**:
- Kết quả tính $\pi$ chính xác với mọi số luồng ($1, 2, 4, 8, \dots$).
- Khi chạy với 1 luồng, thời gian tương đương `pi_single`.
- Speedup tăng rõ rệt khi số luồng $\le$ số core vật lý của máy.

---

## Thành viên C: Approach 3 (Shared Counter with Mutex Lock)

**Vai trò**: Hiện thực phiên bản biến chia sẻ có bảo vệ bằng Mutex để minh chứng chi phí đồng bộ.

**File phụ trách**: `src/pi_mutex.c`, các mục tương ứng trong `report/report.md`.

**Checklist công việc**
- [ ] TODO(C-1) `src/pi_mutex.c`: Đặt `pthread_mutex_lock/unlock` bao quanh mỗi lần tăng biến chia sẻ `shared_inside` bên trong vòng lặp.
- [ ] TODO(C-2) `src/pi_mutex.c`: Khởi tạo và hủy mutex đúng cách (`pthread_mutex_init`, `pthread_mutex_destroy`).
- [ ] TODO(C-3) `src/pi_mutex.c`: Kiểm tra không bị race condition bằng cách đối chiếu kết quả với `pi_local` cùng seed.
- [ ] TODO(C-4) `report/report.md`: Viết Mục 2.3 — Thiết kế Approach 3 (Shared counter, cơ chế lock).
- [ ] TODO(C-5) `report/report.md`: Viết Mục 5.2.D — Phân tích nguyên nhân Approach 3 suy giảm hiệu năng nghiêm trọng (tranh chấp lock, serialization).
- [ ] TODO(C-6) `report/report.md`: Viết Mục 6.3 — Đánh giá so sánh trực tiếp Local Accumulation vs Shared Counter.

**Phụ thuộc**: `src/common.h` của QVinh (Thành viên A); số liệu của Thành viên B để so sánh.
**Định nghĩa hoàn thành (DoD)**:
- Kết quả $\pi$ chính xác, không bị race condition.
- Thời gian thực thi tăng vọt (chậm hơn rõ rệt) so với `pi_local` khi tăng số luồng.

---

## Thành viên D: Biến Thể Đồng Bộ (Mutex theo chu kỳ k & Atomic)

**Vai trò**: Khảo sát tối ưu hóa cơ chế đồng bộ qua gom nhóm (Batching) và chỉ thị nguyên tử (Atomic).

**File phụ trách**: `src/pi_mutex_k.c`, `src/pi_atomic.c`, các mục tương ứng trong `report/report.md`.

**Checklist công việc**
- [ ] TODO(D-1) `src/pi_mutex_k.c`: Đếm tích lũy cục bộ, chỉ lock và cộng dồn vào `shared_inside` mỗi chu kỳ $k$ điểm.
- [ ] TODO(D-2) `src/pi_mutex_k.c`: Xử lý triệt để phần dư cuối cùng sau vòng lặp, không làm mất điểm.
- [ ] TODO(D-3) `src/pi_mutex_k.c`: Kiểm tra: khi $k = 1$ hành vi như `pi_mutex`, khi $k$ rất lớn hành vi tiệm cận `pi_local`.
- [ ] TODO(D-4) `src/pi_atomic.c`: Sử dụng hàm nguyên tử `__atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED)`.
- [ ] TODO(D-5) `src/pi_atomic.c`: Viết ghi chú phân tích so sánh Mutex vs Atomic vs Local ở đầu file.
- [ ] TODO(D-6) `report/report.md`: Viết Mục 2.4 — Thiết kế biến thể Batching Mutex ($k$) và Lock-free Atomic.
- [ ] TODO(D-7) `report/report.md`: Viết Mục 6.1 — Phân tích chi tiết thí nghiệm đánh đổi tần suất đồng bộ theo $k$.

**Phụ thuộc**: `src/common.h` của QVinh (Thành viên A) và cấu trúc của Thành viên C.
**Định nghĩa hoàn thành (DoD)**:
- Kết quả tính $\pi$ chính xác với mọi giá trị $k$ ($1, 10, 100, 1000, \dots$) và bản `pi_atomic`.
- Dữ liệu đo cho thấy thời gian giảm dần khi $k$ tăng theo hàm mũ.

---

## Thành viên E: Tự Động Hóa Benchmark, Vẽ Đồ Thị & Báo Cáo Tổng Hợp

**Vai trò**: Tự động hóa đo lường, xử lý số liệu thống kê, trực quan hóa và tổng hợp báo cáo.

**File phụ trách**: `scripts/run_bench.sh`, `scripts/run_tradeoff.sh`, `scripts/plot.py`, `report/environment.md`, `report/report.md`.

**Checklist công việc**
- [ ] TODO(E-1) `scripts/run_bench.sh`: Tự động lặp qua approach $\times$ threads $\times 5$ lần, xuất đúng chuẩn CSV.
- [ ] TODO(E-2) `scripts/run_bench.sh`: Ghi nhận thông tin phần cứng (`lscpu`, `uname -a`) vào `report/environment.md`.
- [ ] TODO(E-3) `scripts/run_bench.sh`: Hỗ trợ tùy biến tham số ($N$, danh sách thread, số lần lặp) qua biến môi trường.
- [ ] TODO(E-4) `scripts/run_bench.sh`: Bổ sung bước warm-up ban đầu để ổn định cache CPU.
- [ ] TODO(E-5) `scripts/run_tradeoff.sh`: Chạy thí nghiệm 6.1 với các giá trị $k$, xuất ra `results/tradeoff_k.csv`.
- [ ] TODO(E-6) `scripts/run_tradeoff.sh`: Tái sử dụng số liệu cho thí nghiệm 6.3 mà không phải chạy lặp lại.
- [ ] TODO(E-7) `scripts/plot.py`: Đọc CSV, tính toán trung bình (mean) và độ lệch chuẩn (std).
- [ ] TODO(E-8) `scripts/plot.py`: Tính Speedup ($T_{\text{single}} / T_n$) và Parallel Efficiency ($\text{Speedup} / n$).
- [ ] TODO(E-9) `scripts/plot.py`: Vẽ và lưu 3 đồ thị (time, speedup, efficiency) cùng biểu đồ so sánh vào `plots/`.
- [ ] TODO(E-10) `scripts/plot.py`: Vẽ đồ thị thí nghiệm 6.1 (thời gian theo $k$ với trục log).
- [ ] TODO(E-11) `scripts/plot.py`: Đánh dấu vạch thẳng đứng tham chiếu số core CPU vật lý trên đồ thị.
- [ ] TODO(E-12) `report/report.md`: Viết Mục 1 — Giới thiệu bài lab và môi trường thực nghiệm.
- [ ] TODO(E-13) `report/report.md`: Trình bày Mục 4 — Bảng tổng hợp số liệu và chèn các biểu đồ từ `plots/`.
- [ ] TODO(E-14) `report/report.md`: Viết Mục 5.2.E — Phân tích sự suy giảm hiệu suất song song (Parallel Efficiency).
- [ ] TODO(E-15) `report/report.md`: Viết Mục 7 — Tổng kết bài lab, tổng hợp Best Practices từ cả 5 thành viên.

**Phụ thuộc**: Cần các binary hoàn thiện của A, B, C, D để chạy benchmark chính thức.
**Định nghĩa hoàn thành (DoD)**:
- Lệnh `make bench && make plot` thực thi trơn tru, tự động sinh đầy đủ CSV và biểu đồ PNG.

---

## Kế Hoạch 7 Ngày (Timeline)

| Ngày | Kế hoạch thực hiện |
|---|---|
| **Ngày 1** | Họp nhóm, thống nhất hợp đồng. Mỗi người tạo branch riêng. **QVinh (A)** đẩy `src/common.h` |
| **Ngày 2** | **QVinh (A)** hoàn thiện `pi_single.c`. **B, C, D** cài đặt thuật toán của mình. **E** kiểm thử pipeline với số liệu mẫu |
| **Ngày 3** | Toàn bộ code hoàn tất, kiểm tra $\pi \approx 3.1416$ qua `make check`. Tiến hành **Review chéo** |
| **Ngày 4–5** | **E** tiến hành chạy benchmark chính thức trên một máy cấu hình cố định. Toàn nhóm kiểm tra tính hợp lệ của dữ liệu |
| **Ngày 5–6** | Mỗi thành viên viết phần phân tích của mình trong `report/report.md` dựa trên dữ liệu thực tế |
| **Ngày 7** | Đọc soát lỗi toàn diện, định dạng lại báo cáo, đóng gói mã nguồn và nghiệm thu |

## Quy Trình Review Chéo (Ngày 3)

- **QVinh (A)** review code của **B**: Soi chia phần dư `split_work`, chống False Sharing với `padded_u64`.
- **B** review code của **C**: Soi phạm vi khóa Critical Section, khởi tạo/hủy mutex đúng chuẩn.
- **C** review code của **D**: Soi xử lý phần dư chu kỳ $k$ trong `pi_mutex_k.c`, kiểm tra bộ nhớ `pi_atomic.c`.
- **D** review code của **E**: Soi kịch bản benchmark bash, logic parse dữ liệu và tính toán Speedup trong `plot.py`.
- **E** review code của **QVinh (A)**: Soi giao ước `common.h`, logic `rand01`, `xorshift64` và định dạng `print_csv`.

---

## Danh Mục Kiểm Tra Nộp Bài (Submission Checklist)

- [ ] Toàn bộ mã nguồn 5 chương trình C biên dịch sạch sẽ không cảnh báo (`make` thành công).
- [ ] Các tệp dữ liệu thực nghiệm: `results/results.csv` và `results/tradeoff_k.csv`.
- [ ] Toàn bộ đồ thị trực quan hóa trong thư mục `plots/` (`time`, `speedup`, `efficiency`, `tradeoff_k`, `comparison`).
- [ ] Báo cáo `report/report.md` hoàn chỉnh cả 7 mục, nội dung phân tích sâu sắc và đầy đủ luận cứ.
- [ ] Tệp `report/environment.md` ghi nhận chính xác phần cứng và môi trường chạy thử nghiệm.
