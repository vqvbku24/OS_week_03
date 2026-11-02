/**
 * pi_mutex_k.c - Trade-off 6.1: Mutex synchronization frequency (batching every k points)
 * Nguoi phu trach: Thanh vien D
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

void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);

    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(D-1, D-2): worker trong pi_mutex_k dang dong bo theo chu ky k.\n");
    }

    uint64_t local_inside = 0;
    uint64_t k_batch = tdata->k;
    if (k_batch == 0) k_batch = 1;

    // TODO(D-1): Moi thread dem local_inside, cu moi khi local_inside dat k_batch diem thi lock -> cong vao shared_inside -> unlock -> reset local_inside (Thanh vien D).
    // TODO(D-2): Xu ly dung phan du con lai sau khi ket thuc vong lap, tranh lam mat diem khi local_inside con du < k (Thanh vien D).
    // TODO(D-3): Kiem tra tinh chat dac biet: khi k = 1 hanh vi tuong duong pi_mutex; khi k rat lon (>= count) hanh vi tuong duong pi_local (Thanh vien D).
    // Goi y:
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) {
    //           local_inside++;
    //           if (local_inside >= k_batch) {
    //               pthread_mutex_lock(&lock);
    //               shared_inside += local_inside;
    //               pthread_mutex_unlock(&lock);
    //               local_inside = 0;
    //           }
    //       }
    //   }
    //   if (local_inside > 0) {
    //       pthread_mutex_lock(&lock);
    //       shared_inside += local_inside;
    //       pthread_mutex_unlock(&lock);
    //   }
    for (uint64_t i = 0; i < tdata->count; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            local_inside++;
            if (local_inside >= k_batch) {
                pthread_mutex_lock(&lock);
                shared_inside += local_inside;
                pthread_mutex_unlock(&lock);
                local_inside = 0;
            }
        }
    }

    // Xu ly phan du cuoi
    if (local_inside > 0) {
        pthread_mutex_lock(&lock);
        shared_inside += local_inside;
        pthread_mutex_unlock(&lock);
        local_inside = 0;
    }

    return NULL;
}

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Loi: Khong the khoi tao pthread_mutex.\n");
        exit(EXIT_FAILURE);
    }

    shared_inside = 0;

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t)cfg.threads);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * (size_t)cfg.threads);

    if (!threads || !args) {
        fprintf(stderr, "Loi: Khong the cap phat bo nho cho luong.\n");
        exit(EXIT_FAILURE);
    }

    double t_start = now_sec();

    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        args[i].k = cfg.k;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    for (int i = 0; i < cfg.threads; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi = 4.0 * (double)shared_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    pthread_mutex_destroy(&lock);
    free(threads);
    free(args);

    // Dinh dang ten approach: mutex_k<k> (vi du: mutex_k1000)
    char approach_name[64];
    snprintf(approach_name, sizeof(approach_name), "mutex_k%" PRIu64, cfg.k);

    print_csv(approach_name, cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
