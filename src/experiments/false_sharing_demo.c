
/**
 * false_sharing_demo.c
 * Supporting experiment for Approach 2 (Member B)
 *
 * local    : local counter, written once at the end.
 * padded   : each counter occupies its own cache line.
 * unpadded : adjacent counters may share a cache line.
 */

#include "../common.h"

_Static_assert(sizeof(padded_u64) == CACHE_LINE,
               "padded_u64 must fill one cache line");

enum { LOCAL, PADDED, UNPADDED };

typedef struct {
    int id, mode;
    uint64_t count, seed;
    volatile padded_u64 *padded;
    volatile uint64_t *packed;
} arg_t;

static void *worker(void *p) {
    arg_t *a = p;

    uint64_t state = seed_for_thread(a->seed, a->id);
    uint64_t local_inside = 0;

    for (uint64_t i = 0; i < a->count; i++) {
        double x = rand01(&state);
        double y = rand01(&state);

        if (x * x + y * y <= 1.0) {
            if (a->mode == LOCAL) {
                local_inside++;
            } else if (a->mode == PADDED) {
                a->padded[a->id].value++;
            } else {
                a->packed[a->id]++;
            }
        }
    }

    if (a->mode == LOCAL) {
        a->padded[a->id].value = local_inside;
    } else if (a->mode == UNPADDED) {
        a->padded[a->id].value = a->packed[a->id];
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 6) {
        fprintf(stderr,
                "Usage: %s <total> <threads> <seed> <run> "
                "<local|padded|unpadded>\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_t cfg = parse_args(5, argv);

    if (cfg.total == 0) {
        fprintf(stderr, "Error: total_points must be positive.\n");
        return EXIT_FAILURE;
    }

    int mode;

    if (!strcmp(argv[5], "local")) {
        mode = LOCAL;
    } else if (!strcmp(argv[5], "padded")) {
        mode = PADDED;
    } else if (!strcmp(argv[5], "unpadded")) {
        mode = UNPADDED;
    } else {
        fprintf(stderr, "Error: Invalid mode.\n");
        return EXIT_FAILURE;
    }

    int n = cfg.threads;

    pthread_t *th = malloc(sizeof(*th) * (size_t)n);
    arg_t *args = malloc(sizeof(*args) * (size_t)n);

    padded_u64 *padded = aligned_alloc(
        CACHE_LINE,
        sizeof(padded_u64) * (size_t)n
    );

    size_t packed_bytes =
        ((sizeof(uint64_t) * (size_t)n + CACHE_LINE - 1)
        / CACHE_LINE) * CACHE_LINE;

    uint64_t *packed = aligned_alloc(
        CACHE_LINE, packed_bytes
    );

    if (!th || !args || !padded || !packed) {
        fprintf(stderr, "Error: Failed to allocate memory.\n");

        free(th);
        free(args);
        free(padded);
        free(packed);

        return EXIT_FAILURE;
    }

    memset(padded, 0, sizeof(padded_u64) * (size_t)n);
    memset(packed, 0, packed_bytes);

    double t0 = now_sec();

    int created = 0;

    for (int i = 0; i < n; i++) {
        args[i] = (arg_t){
            i, mode,
            split_work(cfg.total, n, i),
            cfg.seed, padded, packed
        };

        int rc = pthread_create(
            &th[i], NULL, worker, &args[i]
        );

        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            break;
        }

        created++;
    }

    uint64_t inside = 0;

    for (int i = 0; i < created; i++) {
        int rc = pthread_join(th[i], NULL);

        if (rc != 0) {
            fprintf(stderr, "pthread_join: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }

        inside += padded[i].value;
    }

    if (created != n) {
        free(th);
        free(args);
        free(padded);
        free(packed);

        return EXIT_FAILURE;
    }

    double pi = 4.0 * (double)inside / (double)cfg.total;
    double t1 = now_sec();

    print_csv(argv[5], n, cfg.run, t1 - t0, pi);

    free(th);
    free(args);
    free(padded);
    free(packed);

    return 0;
}

