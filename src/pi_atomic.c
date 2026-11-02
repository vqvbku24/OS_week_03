/**
 * pi_atomic.c - Multi-thread with Atomic Operations (Lock-free approach)
 * Assigned Member: Member D
 *
 * TODO(D-5): Document comparison among Mutex vs Atomic vs Local in header comment (Member D).
 *
 * SYNCHRONIZATION METHODS COMPARISON:
 * 1. Mutex (pi_mutex.c):
 *    - Operates at OS / pthread runtime level.
 *    - Under high contention, threads are blocked and put to sleep, causing context switches
 *      that waste thousands of CPU cycles per sleep/wake transition. Slowest under high contention.
 *
 * 2. Atomic (pi_atomic.c):
 *    - Utilizes hardware-level atomic instructions (such as LOCK XADD on x86).
 *    - Avoids kernel transitions and context switches, significantly faster than mutexes.
 *    - However, repeatedly atomic-adding into a single shared address forces cores to invalidate
 *      each other's cache lines (Cache Coherence Bus Traffic), limiting parallel scalability.
 *
 * 3. Local Accumulation (pi_local.c):
 *    - No shared counter, no lock, and no atomic operations inside the hot computation loop.
 *    - Each thread writes exclusively to its dedicated L1/L2 cache line.
 *    - Requires only a single fast Reduce step after joining.
 *    - Optimal strategy in both execution wall-clock time and parallel speedup.
 */

#include "common.h"

// Global shared counter updated with atomic primitives
static uint64_t shared_inside = 0;

typedef struct {
    int id;
    uint64_t count;
    uint64_t seed;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);

    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(D-4): worker in pi_atomic is running using __atomic_fetch_add.\n");
    }

    // TODO(D-4): Use __atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED) in place of mutex on each circle hit (Member D).
    // Hint:
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) {
    //           __atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED);
    //       }
    //   }
    // Acceptance criteria: Atomic increment guarantees correct sum without race conditions or lock overhead.
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

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);

    if (!threads || !args) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        exit(EXIT_FAILURE);
    }

    double t_start = now_sec();

    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < cfg.threads; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi = 4.0 * (double)shared_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    free(threads);
    free(args);

    // Output single CSV row
    print_csv("atomic", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
