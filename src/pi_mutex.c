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

    /* Protect every shared_inside update with a mutex lock to isolate synchronization overhead. */
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

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Error: Failed to initialize pthread_mutex.\n");
        exit(EXIT_FAILURE);
    }

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);

    if (!threads || !args) {
        fprintf(stderr, "Error: Failed to allocate memory for threads.\n");
        free(threads);
        free(args);
        pthread_mutex_destroy(&lock);
        exit(EXIT_FAILURE);
    }

    // Time computational workload from thread creation to completion of join
    double t_start = now_sec();

    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            fprintf(stderr, "Error: Failed to create thread %d.\n", i);
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            pthread_mutex_destroy(&lock);
            free(threads);
            free(args);
            exit(EXIT_FAILURE);
        }
    }

    int join_failed = 0;
    for (int i = 0; i < cfg.threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error: Failed to join thread %d.\n", i);
            join_failed = 1;
        }
    }

    if (join_failed) {
        pthread_mutex_destroy(&lock);
        free(threads);
        free(args);
        exit(EXIT_FAILURE);
    }

    double pi = (cfg.total > 0) ? (4.0 * (double)shared_inside / (double)cfg.total) : 0.0;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Destroy mutex after all worker threads have joined
    pthread_mutex_destroy(&lock);

    free(threads);
    free(args);

    // Output single CSV row
    print_csv("mutex", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
