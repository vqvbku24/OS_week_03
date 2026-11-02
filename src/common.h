#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define CACHE_LINE 64

// Cau truc padding 64 bytes de ngan ngua hien tuong false sharing giua cac luong
typedef struct {
    uint64_t value;
    char pad[CACHE_LINE - sizeof(uint64_t)];
} padded_u64;

// Cau truc luu thong so cau hinh dong lenh
typedef struct {
    uint64_t total;
    int threads;
    uint64_t seed;
    int run;
    uint64_t k;
} config_t;

// In dung mot dong CSV duy nhat ra stdout theo hop dong:
// approach,threads,run,time_sec,pi_estimate
static inline void print_csv(const char *approach, int threads, int run, double time_sec, double pi) {
    printf("%s,%d,%d,%.6f,%.9f\n", approach, threads, run, time_sec, pi);
}

// Parse tham so dong lenh thong nhat: ./prog <total_points> <num_threads> <seed> <run_id> [k]
static inline config_t parse_args(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, "Su dung: %s <total_points> <num_threads> <seed> <run_id> [k]\n", argv[0]);
        fprintf(stderr, "  total_points: Tong so diem can tao (vi du: 100000000)\n");
        fprintf(stderr, "  num_threads : So luong luong (vi du: 1, 2, 4, 8...)\n");
        fprintf(stderr, "  seed        : Seed goc ngau nhien (vi du: 12345)\n");
        fprintf(stderr, "  run_id      : So thu tu lan chay (vi du: 1, 2, 3...)\n");
        fprintf(stderr, "  k           : Chu ky dong bo mutex (tuy chon, mac dinh: 1000)\n");
        exit(EXIT_FAILURE);
    }

    config_t cfg;
    cfg.total   = (uint64_t)strtoull(argv[1], NULL, 10);
    cfg.threads = atoi(argv[2]);
    cfg.seed    = (uint64_t)strtoull(argv[3], NULL, 10);
    cfg.run     = atoi(argv[4]);
    cfg.k       = (argc >= 6) ? (uint64_t)strtoull(argv[5], NULL, 10) : 1000ULL;

    if (cfg.threads <= 0) cfg.threads = 1;
    return cfg;
}

// Lay thoi gian hien tai tinh bang giay
static inline double now_sec(void) {
    // TODO(A-1): Dung clock_gettime(CLOCK_MONOTONIC) de lay thoi gian chinh xac (QVinh - Thanh vien A).
    // Goi y:
    //   struct timespec ts;
    //   clock_gettime(CLOCK_MONOTONIC, &ts);
    //   return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
    // Tieu chi: Tra ve gia tri thoi gian chinh xac voi do phan giai cao, khong bi anh huong boi thay doi gio he thong.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-1): now_sec() chua hoan thien, dang dung clock_gettime mac dinh hoac tra ve 0.0.\n");
    }
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
    }
    return 0.0;
}

// Thuat toan tao so ngau nhien nhanh xorshift64 (tranh dung rand() toan cuc)
static inline uint64_t xorshift64(uint64_t *state) {
    // TODO(A-2): Cai dat xorshift64 chuan voi cac phep dich bit (13, 7, 17); luu y xu ly state != 0 (QVinh - Thanh vien A).
    // Goi y:
    //   uint64_t x = *state;
    //   if (x == 0) x = 0x853c49e6748fea9bULL;
    //   x ^= x << 13;
    //   x ^= x >> 7;
    //   x ^= x << 17;
    //   return *state = x;
    // Tieu chi: Sinh so ngau nhien 64-bit nhanh, chu ky 2^64-1, khong bi treo neu state = 0.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-2): xorshift64() chua cai dat xong.\n");
    }
    if (*state == 0) *state = 1;
    *state = *state * 6364136223846793005ULL + 1;
    return *state;
}

// Chuyen so ngau nhien sang kieu double trong nua khoang [0, 1)
static inline double rand01(uint64_t *state) {
    // TODO(A-3): Chuyen ket qua tu xorshift64 sang double trong [0,1) bang cach lay 53 bit cao (QVinh - Thanh vien A).
    // Goi y:
    //   return (double)(xorshift64(state) >> 11) * (1.0 / 9007199254740992.0); // 2^53 = 9007199254740992.0
    // Tieu chi: Tra ve gia tri double dong nhat trong khoang [0, 1).
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-3): rand01() chua cai dat xong.\n");
    }
    return (double)(xorshift64(state) & 0xFFFFFFFFFFFFFULL) / (double)0x10000000000000ULL;
}

// Chia phan cong viec cho thread thu i tren tong so nthreads
static inline uint64_t split_work(uint64_t total, int nthreads, int i) {
    // TODO(A-4): Chia deu tong so diem cho cac thread, thread cuoi cung (i == nthreads - 1) nhan them phan du (QVinh - Thanh vien A).
    // Goi y:
    //   uint64_t base = total / nthreads;
    //   return (i == nthreads - 1) ? (base + (total % nthreads)) : base;
    // Tieu chi: Tong so diem tat ca cac thread nhan duoc phai chinh xac bang total.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-4): split_work() chua tinh phan du cho thread cuoi.\n");
    }
    uint64_t base = total / (uint64_t)nthreads;
    return (i == nthreads - 1) ? (base + (total % (uint64_t)nthreads)) : base;
}

// Khoi tao seed doc lap cho moi luong
static inline uint64_t seed_for_thread(uint64_t base, int i) {
    // TODO(A-5): Tra ve seed cho thread i dua vao base seed, dam bao seed khac 0 va co tron bit neu can (QVinh - Thanh vien A).
    // Goi y:
    //   uint64_t s = base + (uint64_t)i * 0x9e3779b97f4a7c15ULL;
    //   return (s == 0) ? 1 : s;
    // Tieu chi: Moi thread nhan seed khac biet, khong bi trung lap chuoi so ngau nhien va seed != 0.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-5): seed_for_thread() can dam bao tron bit doc lap.\n");
    }
    uint64_t s = base + (uint64_t)i * 0x9e3779b97f4a7c15ULL;
    return (s == 0) ? 1 : s;
}

#endif // COMMON_H
