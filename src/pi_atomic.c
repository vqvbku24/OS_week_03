/**
 * pi_atomic.c - Multi-thread with Atomic Operations (Lock-free approach)
 * Nguoi phu trach: Thanh vien D
 *
 * TODO(D-5): Ghi chu so sanh Mutex vs Atomic vs Local trong comment dau file (Thanh vien D).
 *
 * SO SANH CAC PHUONG PHAP DONG BO:
 * 1. Mutex (pi_mutex.c):
 *    - Hoat dong o cap do OS / pthread runtime.
 *    - Khi xay ra tranh chap (contention), cac thread bi block, gay context switch, tieu ton
 *      hang ngan chu ky CPU cho moi lan ngu/thuc day. Day la phuong phap cham nhat khi tranh chap cao.
 *
 * 2. Atomic (pi_atomic.c):
 *    - Su dung chi thi phan cung nguyen tu truc tiep (nhu LOCK XADD tren x86).
 *    - Khong can vao kernel, khong bi context switch nen nhanh hon dang ke so voi mutex.
 *    - Tuy nhien, khi moi diem trong deu atomic add vao cung mot bien, cac core lien tuc tranh chap
 *      quyen ghi doc quyen len cache line chua bien do (Cache Invalidation & Bus Traffic).
 *      Hien tuong nay van gay nghen duong truyen cache coherence va han che kha nang scale.
 *
 * 3. Local Accumulation (pi_local.c):
 *    - Khong chia se bien, khong lock, khong atomic trong vong lap chinh.
 *    - Moi thread doc quyen ghi vao cache L1/L2 cuc bo cua core minh.
 *    - Chi can mot buoc Reduce don gian cuoi cung.
 *    - Day la giai phap toi uu nhat ca ve thoi gian thuc thi lan parallel speedup.
 */

#include "common.h"

// Bien chia se toan cuc dung cho phep toan nguyen tu
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
        fprintf(stderr, "STUB(D-4): worker trong pi_atomic dang su dung __atomic_fetch_add.\n");
    }

    // TODO(D-4): Dung __atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED) thay cho mutex moi khi diem roi vao hinh tron (Thanh vien D).
    // Goi y:
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) {
    //           __atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED);
    //       }
    //   }
    // Tieu chi: Phep cong duoc bao dam nguyen tu (atomic), khong bi race condition, tranh lock overhead.
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
        fprintf(stderr, "Loi: Khong the cap phat bo nho cho luong.\n");
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

    // In dung 1 dong CSV ra stdout
    print_csv("atomic", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
