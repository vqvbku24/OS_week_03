# Môi Trường Thực Nghiệm (Environment)

Phần này ghi nhận thông tin phần cứng, hệ điều hành và trình biên dịch được sử dụng để chạy benchmark chuẩn (do **Thành viên E** ghi nhận qua script `scripts/run_bench.sh`).

## 1. Thông tin phần cứng & CPU
<!-- TODO(E-2): scripts/run_bench.sh sẽ tự động xuất lscpu / uname -a vào đây -->
- **CPU Model**: Sẽ được cập nhật khi chạy benchmark
- **Số Cores vật lý / Logical Cores**: 
- **L1/L2/L3 Cache**: 
- **RAM**: 

## 2. Hệ điều hành
- **Hệ điều hành / OS Version**: 
- **Kernel Version**: 

## 3. Trình biên dịch & Cờ tối ưu
- **Compiler**: gcc (hỗ trợ C11, pthread)
- **Compiler Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`
