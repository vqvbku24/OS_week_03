/**
 * pi_atomic.c - Multi-thread with Atomic Operations (Lock-free approach)
 * Assigned Member: Member D
 *
 * TODO(D-5) [done]: Comparison among Mutex vs Atomic vs Local.
 * (Expected behaviour from theory; the report confirms or rejects it with measured data.)
 *
 * 1. Mutex (pi_mutex.c):
 *    - pthread_mutex on Linux/glibc is futex-based. An uncontended lock/unlock stays in
 *      user space (a few atomic instructions). Under contention a waiting thread may spin
 *      briefly and is then put to sleep with futex_wait and woken by futex_wake, which
 *      costs system calls and usually a context switch.
 *    - Only one thread can be inside the critical section at a time, so every update of
 *      shared_inside is serialized. The point generation outside the lock still runs in
 *      parallel, but with one lock per hit the serialized part dominates as threads grow.
 *
 * 2. Atomic (pi_atomic.c):
 *    - One hardware atomic read-modify-write per hit: a single instruction on x86-64
 *      (`lock add/xadd`) and ARMv8.1 LSE (`ldadd`), or an `ldxr/stxr` retry loop on older ARM64. A thread never
 *      blocks waiting for another thread (it can still be preempted by the OS scheduler
 *      like any thread), so it is expected to be considerably faster than the mutex.
 *    - It is still not expected to scale: every core writes the same cache line (64 bytes
 *      on typical x86-64, 128 bytes on Apple M-series), so the line keeps moving between
 *      cores' caches (cache-coherence traffic / true sharing). Each atomic must own the line
 *      exclusively, which serializes the updates in hardware.
 *    - __ATOMIC_RELAXED is enough: only the increment itself must be atomic; no ordering
 *      with other memory accesses is needed. The final read happens after pthread_join,
 *      which already provides the happens-before relation.
 *
 * 3. Local Accumulation (pi_local.c):
 *    - The hot loop touches no shared memory: the counter is a thread-local variable
 *      (normally kept in a register by the compiler) and the
 *      PRNG state is thread-private. Each thread writes its result once into its own
 *      cache-line-padded slot, and the main thread sums them after join (Reduce).
 *    - Expected to give the best wall-clock time and the closest-to-linear speedup up to
 *      the number of physical cores.
 *
 * Expected ordering (fastest -> slowest): local ~ mutex_k(large k) < atomic < mutex.
 * Caveat: the hit test here must branch (update only on a hit), while a branch-free
 * local counter avoids branch mispredictions, so part of the local-vs-atomic gap can come
 * from code generation rather than synchronization alone.
 */

#include "common.h"

// Global shared counter, updated only through atomic built-ins.
static uint64_t shared_inside = 0;

typedef struct {
    int id;
    uint64_t count;
    uint64_t seed;
} thread_arg_t;

static void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);   // thread-private PRNG state

    // TODO(D-4) [done]: one relaxed atomic fetch-add per circle hit, no mutex.
    for (uint64_t i = 0; i < tdata->count; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            __atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED);
        }
    }

    return NULL;
}

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    if (cfg.total == 0) {
        fprintf(stderr, "Error: total_points must be >= 1.\n");   // avoid 0/0 = NaN
        return EXIT_FAILURE;
    }

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);
    if (!threads || !args) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        free(threads);                 // free(NULL) is a no-op
        free(args);
        return EXIT_FAILURE;
    }

    int ok = 1;
    int created = 0;
    double t_start = now_sec();

    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "Error: pthread_create failed for thread %d.\n", i);
            ok = 0;
            break;                     // join the threads already started, then fail
        }
        created++;
    }

    for (int i = 0; i < created; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            // The worker may still be running: reading the counter, destroying the mutex or
            // freeing args now would be undefined behaviour, so terminate immediately.
            fprintf(stderr, "Error: pthread_join failed for thread %d.\n", i);
            exit(EXIT_FAILURE);
        }
    }

    uint64_t total_inside = __atomic_load_n(&shared_inside, __ATOMIC_RELAXED);
    double pi = 4.0 * (double)total_inside / (double)cfg.total;
    double time_sec = now_sec() - t_start;

    free(threads);
    free(args);

    if (!ok) return EXIT_FAILURE;      // never print a CSV row from an incomplete run

    print_csv("atomic", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
