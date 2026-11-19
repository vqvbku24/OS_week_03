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

// 64-byte padded structure to prevent false sharing between threads on adjacent cache lines
typedef struct {
    uint64_t value;
    char pad[CACHE_LINE - sizeof(uint64_t)];
} padded_u64;

// Structure to store parsed command-line arguments
typedef struct {
    uint64_t total;
    int threads;
    uint64_t seed;
    int run;
    uint64_t k;
} config_t;

// Output exactly ONE CSV line to stdout conforming to contract:
// approach,threads,run,time_sec,pi_estimate
static inline void print_csv(const char *approach, int threads, int run, double time_sec, double pi) {
    printf("%s,%d,%d,%.6f,%.9f\n", approach, threads, run, time_sec, pi);
}

// Parse command line arguments: ./prog <total_points> <num_threads> <seed> <run_id> [k]
static inline config_t parse_args(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <total_points> <num_threads> <seed> <run_id> [k]\n", argv[0]);
        fprintf(stderr, "  total_points: Total Monte Carlo points to generate (e.g., 100000000)\n");
        fprintf(stderr, "  num_threads : Number of execution threads (e.g., 1, 2, 4, 8...)\n");
        fprintf(stderr, "  seed        : Base random seed (e.g., 12345)\n");
        fprintf(stderr, "  run_id      : Iteration/run sequence id (e.g., 1, 2, 3...)\n");
        fprintf(stderr, "  k           : Mutex batching frequency (optional, default: 1000)\n");
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

// Get current monotonic timestamp in seconds
static inline double now_sec(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        return 0.0;
    }
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

// Fast 64-bit pseudo-random number generator (xorshift64) to avoid global rand() locks
static inline uint64_t xorshift64(uint64_t *state) {
    uint64_t x = *state;
    if (x == 0) {
        x = 0x853c49e6748fea9bULL;
    }
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *state = x;
    return x;
}

// Convert pseudo-random 64-bit integer to double uniformly in [0, 1) using upper 53 bits
static inline double rand01(uint64_t *state) {
    return (double)(xorshift64(state) >> 11) * (1.0 / 9007199254740992.0); // 2^53 = 9007199254740992.0
}

// Partition total points among nthreads; the last thread receives the remainder
static inline uint64_t split_work(uint64_t total, int nthreads, int i) {
    if (nthreads <= 0 || i < 0 || i >= nthreads) {
        return 0;
    }
    uint64_t base = total / (uint64_t)nthreads;
    uint64_t rem  = total % (uint64_t)nthreads;
    return (i == nthreads - 1) ? (base + rem) : base;
}

// Generate an independent, non-zero per-thread random seed using SplitMix64 mixing
static inline uint64_t seed_for_thread(uint64_t base, int i) {
    uint64_t z = base + (uint64_t)i + 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);
    return (z == 0) ? 0x853c49e6748fea9bULL : z;
}

#endif // COMMON_H
