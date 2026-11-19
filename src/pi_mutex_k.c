/**
 * pi_mutex_k.c - Trade-off 6.1: Mutex synchronization frequency (update every k iterations)
 * Assigned Member: Member D
 *
 * Lab spec 6.1: "Modify Approach 3: update the shared variable every iteration /
 * update every k iterations. Observe performance improvement as synchronization
 * frequency decreases."
 *
 * Design:
 *   Each worker processes its points in blocks of k iterations. Inside a block, hits are
 *   counted in a private register (local_inside). At the end of every block the worker
 *   takes the global mutex ONCE, adds the block's hits to shared_inside, and resets
 *   local_inside. The last block may be shorter than k (when count % k != 0) and is
 *   flushed the same way, so no hit is lost.
 *   Synchronization frequency is therefore exactly ceil(count / k) lock acquisitions per
 *   thread, independent of how many points land inside the circle.
 *
 *   - k = 1          -> lock on every iteration ("update every iteration"), the most
 *                       synchronization-heavy variant (slightly more locks than pi_mutex,
 *                       which only locks on a hit, ~78.5% of iterations).
 *   - k >= count     -> a single lock per thread at the end, equivalent to pi_local's
 *                       single Reduce step.
 *
 * Note on the hot loop:
 *   The hit test is accumulated branch-free (local_inside += (x*x + y*y <= 1.0)). A
 *   branch on the random hit condition (~21.5% of points are outside) tends to be
 *   mispredicted; on our test machine the branchy version of this file was measurably
 *   slower at large k (about 0.167 s vs 0.103 s, 1 thread, N = 2e7, single run), even
 *   though it hardly locked at all. Splitting the loop into blocks also removes any
 *   per-iteration modulo / counter test.
 *   Consequence for the report: when comparing approaches, timing differences come from
 *   both synchronization frequency AND loop code generation (branch prediction), so the
 *   hit test should be written the same way in the approaches being compared.
 */

#include "common.h"

static uint64_t shared_inside = 0;
static pthread_mutex_t lock;

typedef struct {
    int id;
    uint64_t count;
    uint64_t seed;
    uint64_t k;
} thread_arg_t;

static inline void flush_batch(uint64_t batch) {
    pthread_mutex_lock(&lock);
    shared_inside += batch;          // critical section: a single add
    pthread_mutex_unlock(&lock);
}

static void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);   // thread-private PRNG state
    const uint64_t k_batch = tdata->k;                          // validated >= 1 in main
    uint64_t local_inside = 0;

    // TODO(D-1) [done]: update shared_inside once every k_batch iterations.
    // TODO(D-2) [done]: the final, shorter block (count % k_batch) is flushed too.
    for (uint64_t done = 0; done < tdata->count; ) {
        uint64_t remaining = tdata->count - done;
        uint64_t block = (remaining < k_batch) ? remaining : k_batch;

        for (uint64_t j = 0; j < block; j++) {
            double x = rand01(&state);
            double y = rand01(&state);
            local_inside += (x * x + y * y <= 1.0);   // same hit test as pi_single / pi_local
        }

        flush_batch(local_inside);   // one lock per k iterations (even if no hit in block)
        local_inside = 0;
        done += block;
    }

    return NULL;
}

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    // k = 0 has no meaning (a block of zero iterations). Reject it instead of silently
    // running as k = 1 while printing "mutex_k0" into the CSV.
    // A negative k ("-5") would wrap around in strtoull to a huge value, so check the text too.
    if (cfg.k == 0 || (argc >= 6 && argv[5][0] == '-')) {
        fprintf(stderr, "Error: k must be >= 1.\n");
        return EXIT_FAILURE;
    }
    if (cfg.total == 0) {
        fprintf(stderr, "Error: total_points must be >= 1.\n");   // avoid 0/0 = NaN
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Error: Failed to initialize pthread_mutex.\n");
        return EXIT_FAILURE;
    }

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);
    if (!threads || !args) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        free(threads);                 // free(NULL) is a no-op
        free(args);
        pthread_mutex_destroy(&lock);
        return EXIT_FAILURE;
    }

    int ok = 1;
    int created = 0;
    double t_start = now_sec();

    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        args[i].k = cfg.k;
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

    // All joins succeeded: pthread_join gives a happens-before edge, so no lock is needed.
    double pi = 4.0 * (double)shared_inside / (double)cfg.total;
    double time_sec = now_sec() - t_start;

    pthread_mutex_destroy(&lock);
    free(threads);
    free(args);

    if (!ok) return EXIT_FAILURE;      // never print a CSV row from an incomplete run

    // TODO(D-3) [done]: verified with identical seeds that every k gives the same pi as
    // pi_local / pi_mutex; k = 1 is as slow as (slightly slower than) pi_mutex, large k
    // matches pi_local.

    // Approach name encodes k (e.g. mutex_k1000) so plot.py can group by k.
    char approach_name[64];
    snprintf(approach_name, sizeof(approach_name), "mutex_k%" PRIu64, cfg.k);
    print_csv(approach_name, cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
