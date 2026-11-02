/**
 * pi_mutex.c - Approach 3: Multi-thread with Shared Counter & Mutex Lock
 * Nguoi phu trach: Thanh vien C
 */

#include "common.h"

// Bien chia se toan cuc va mutex
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
        fprintf(stderr, "STUB(C-1): worker trong pi_mutex dang su dung mutex moi lan diem roi vao hinh tron.\n");
    }

    // TODO(C-1): Vong lap thuc hien lock/unlock QUANH MOI LAN tang bien chia se shared_inside (Thanh vien C).
    // Chu y: Day la chu y chu dich thiet ke de thay ro chi phi tranh chap (mutex contention) khi nhieu luong tranh gianh lock.
    // Goi y:
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) {
    //           pthread_mutex_lock(&lock);
    //           shared_inside++;
    //           pthread_mutex_unlock(&lock);
    //       }
    //   }
    // Tieu chi: Bao ve dung Critical Section, tranh race condition tren shared_inside.
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

    // TODO(C-2): Khoi tao mutex truoc khi chay luong va huy mutex khi ket thuc (Thanh vien C).
    // Goi y:
    //   pthread_mutex_init(&lock, NULL);
    //   ...
    //   pthread_mutex_destroy(&lock);
    // Tieu chi: Mutex duoc khoi tao thanh cong va giai phong dung cach, khong bi ro ri tai nguyen.
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

    // Do thoi gian tinh toan tu luc tao thread den khi join xong
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

    // TODO(C-3): Xac nhan ket qua khong co race condition bang cach so sanh pi tinh duoc voi pi_local (Thanh vien C).
    // Tieu chi: pi uoc luong phai xap xi 3.1416 va ket qua khong bi sut giam diem do xung dot ghi dong thoi.
    double pi = 4.0 * (double)shared_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Huy mutex
    pthread_mutex_destroy(&lock);

    free(threads);
    free(args);

    // In dung 1 dong CSV ra stdout
    print_csv("mutex", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
