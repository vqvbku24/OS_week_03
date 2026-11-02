/**
 * pi_mutex.c - Approach 3: Multi-thread with Shared Counter & Mutex Lock
 * Assigned Member: Member C
 */

#include "common.h"

// Global shared counter and mutex lock
static uint64_t shared_inside = 0;
static pthread_mutex_t lock;

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
        fprintf(stderr, "STUB(C-1): worker in pi_mutex is running with mutex lock on each hit.\n");
    }

    // TODO(C-1): Acquire and release lock AROUND EVERY INCREMENT of shared_inside inside the loop (Member C).
    // Note: This deliberate design reveals synchronization overhead (mutex contention) when threads compete for the lock.
    // Hint:
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) {
    //           pthread_mutex_lock(&lock);
    //           shared_inside++;
    //           pthread_mutex_unlock(&lock);
    //       }
    //   }
    // Acceptance criteria: Properly protects Critical Section, preventing race conditions on shared_inside.
    for (uint64_t i = 0; i < tdata->count; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            pthread_mutex_lock(&lock);
            shared_inside++;
            pthread_mutex_unlock(&lock);
        }
    }

    return NULL;
}

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    // TODO(C-2): Correctly initialize mutex before spawning threads and destroy mutex upon completion (Member C).
    // Hint:
    //   pthread_mutex_init(&lock, NULL);
    //   ...
    //   pthread_mutex_destroy(&lock);
    // Acceptance criteria: Mutex initialized without errors and safely reclaimed without resource leaks.
    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Error: Failed to initialize pthread_mutex.\n");
        exit(EXIT_FAILURE);
    }

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);

    if (!threads || !args) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        exit(EXIT_FAILURE);
    }

    // Time computational workload from thread creation to completion of join
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

    // TODO(C-3): Verify no race condition occurred by comparing pi estimate against pi_local with identical seed (Member C).
    // Acceptance criteria: pi estimate must approximate 3.1416 without missing hit counts from concurrency conflicts.
    double pi = 4.0 * (double)shared_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Destroy mutex
    pthread_mutex_destroy(&lock);

    free(threads);
    free(args);

    // Output single CSV row
    print_csv("mutex", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
