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

// Get current timestamp in seconds
static inline double now_sec(void) {
    // TODO(A-1): Use clock_gettime(CLOCK_MONOTONIC) to obtain high-resolution timestamp (QVinh - Member A).
    // Hint:
    //   struct timespec ts;
    //   clock_gettime(CLOCK_MONOTONIC, &ts);
    //   return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
    // Acceptance criteria: Return high-resolution monotonic time without being affected by wall-clock changes.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-1): now_sec() is incomplete, returning default clock_gettime or 0.0.\n");
    }
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
    }
    return 0.0;
}

// Fast 64-bit pseudo-random number generator (xorshift64) to avoid global rand() locks
static inline uint64_t xorshift64(uint64_t *state) {
    // TODO(A-2): Implement standard xorshift64 using bit shifts (13, 7, 17); ensure state != 0 (QVinh - Member A).
    // Hint:
    //   uint64_t x = *state;
    //   if (x == 0) x = 0x853c49e6748fea9bULL;
    //   x ^= x << 13;
    //   x ^= x >> 7;
    //   x ^= x << 17;
    //   return *state = x;
    // Acceptance criteria: Fast 64-bit PRNG with period 2^64-1, handles state == 0 safely.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-2): xorshift64() is running stub PRNG.\n");
    }
    if (*state == 0) *state = 1;
    *state = *state * 6364136223846793005ULL + 1;
    return *state;
}

// Convert pseudo-random 64-bit integer to double in [0, 1)
static inline double rand01(uint64_t *state) {
    // TODO(A-3): Convert xorshift64 output to double in [0, 1) using upper 53 bits (QVinh - Member A).
    // Hint:
    //   return (double)(xorshift64(state) >> 11) * (1.0 / 9007199254740992.0); // 2^53 = 9007199254740992.0
    // Acceptance criteria: Returns uniformly distributed double values in [0, 1).
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-3): rand01() is running stub generator.\n");
    }
    return (double)(xorshift64(state) & 0xFFFFFFFFFFFFFULL) / (double)0x10000000000000ULL;
}

// Partition total points among nthreads; the last thread receives the remainder
static inline uint64_t split_work(uint64_t total, int nthreads, int i) {
    // TODO(A-4): Divide work evenly across threads, last thread (i == nthreads - 1) gets remainder (QVinh - Member A).
    // Hint:
    //   uint64_t base = total / nthreads;
    //   return (i == nthreads - 1) ? (base + (total % nthreads)) : base;
    // Acceptance criteria: Sum of work items across all threads must strictly equal total.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-4): split_work() has been called.\n");
    }
    uint64_t base = total / (uint64_t)nthreads;
    return (i == nthreads - 1) ? (base + (total % (uint64_t)nthreads)) : base;
}

// Generate thread-specific random seed
static inline uint64_t seed_for_thread(uint64_t base, int i) {
    // TODO(A-5): Return distinct seed for thread i based on base seed, ensure seed != 0 and mix bits if needed (QVinh - Member A).
    // Hint:
    //   uint64_t s = base + (uint64_t)i * 0x9e3779b97f4a7c15ULL;
    //   return (s == 0) ? 1 : s;
    // Acceptance criteria: Each thread gets a unique non-zero seed with decorrelated sequences.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(A-5): seed_for_thread() has been called.\n");
    }
    uint64_t s = base + (uint64_t)i * 0x9e3779b97f4a7c15ULL;
    return (s == 0) ? 1 : s;
}

#endif // COMMON_H
