/**
 * pi_local.c - Approach 2: Multi-thread with Local Accumulation (Map-Reduce)
 * Assigned Member: Member B
 */

#include "common.h"

// TODO(B-5): Explain why padding structure (padded_u64) is used in the results array (Member B).
// Explanation:
//   When multiple threads concurrently write to adjacent elements of an array, even though
//   each thread writes to a distinct memory address, if those addresses share a single Cache Line
//   (typically 64 bytes), the CPU must repeatedly invalidate cache lines across cores
//   (Cache Coherence Ping-Pong). This phenomenon is known as False Sharing and severely degrades
//   parallel performance. Using padded_u64 ensures each result occupies its own dedicated
//   64-byte Cache Line, completely eliminating False Sharing.

typedef struct {
    int id;
    uint64_t count;
    uint64_t seed;
    padded_u64 *results;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;

    // TODO(B-2): Initialize independent PRNG state for this thread using seed_for_thread (Member B).
    // Hint:
    //   uint64_t state = seed_for_thread(tdata->seed, tdata->id);
    // Acceptance criteria: Each thread maintains private state without sharing with other threads.
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);

    // TODO(B-1): Use LOCAL VARIABLE local_inside in the calculation loop, NEVER write to results
    // array within the inner loop. Only write once to results[id].value upon completion (Member B).
    // Hint:
    //   uint64_t local_inside = 0;
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) local_inside++;
    //   }
    //   tdata->results[tdata->id].value = local_inside;
    // Acceptance criteria: Extremely fast inner loop with zero shared memory or lock overhead.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(B-1, B-2): worker in pi_local is running.\n");
    }

    uint64_t local_inside = 0;
    for (uint64_t i = 0; i < tdata->count; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            local_inside++;
        }
    }
    tdata->results[tdata->id].value = local_inside;

    return NULL;
}

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);
    padded_u64 *results = malloc(sizeof(padded_u64) * (size_t)cfg.threads);

    if (!threads || !args || !results) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        exit(EXIT_FAILURE);
    }

    // Time computational workload from thread creation to completion of join & reduce
    double t_start = now_sec();

    // TODO(B-3): Partition workload evenly using split_work and spawn threads with pthread_create (Member B).
    // Hint:
    //   for (int i = 0; i < cfg.threads; i++) {
    //       args[i].id = i;
    //       args[i].count = split_work(cfg.total, cfg.threads, i);
    //       args[i].seed = cfg.seed;
    //       args[i].results = results;
    //       pthread_create(&threads[i], NULL, worker, &args[i]);
    //   }
    // Acceptance criteria: Spawns exact number of threads, correctly distributing all points.
    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        args[i].results = results;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    // TODO(B-4): Join all threads, perform Reduce phase (summing results[i].value), and compute pi (Member B).
    // Hint:
    //   uint64_t total_inside = 0;
    //   for (int i = 0; i < cfg.threads; i++) {
    //       pthread_join(threads[i], NULL);
    //       total_inside += results[i].value;
    //   }
    //   double pi = 4.0 * (double)total_inside / (double)cfg.total;
    // Acceptance criteria: Safely awaits all threads, aggregates final count, computes pi ~ 3.1416.
    uint64_t total_inside = 0;
    for (int i = 0; i < cfg.threads; i++) {
        pthread_join(threads[i], NULL);
        total_inside += results[i].value;
    }

    double pi = 4.0 * (double)total_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Free resources
    free(threads);
    free(args);
    free(results);

    // Print single CSV row to stdout
    print_csv("local", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
