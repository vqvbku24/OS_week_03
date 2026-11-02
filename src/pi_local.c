/**
 * pi_local.c - Approach 2: Multi-thread with Local Accumulation (Map-Reduce)
 * Nguoi phu trach: Thanh vien B
 */

#include "common.h"

// TODO(B-5): Giai thich vi sao dung cau truc padding (padded_u64) trong mang results (Thanh vien B).
// Giai thich:
//   Khi cac luong ghi dong thoi vao cac phan tu lien ke cua mot mang, mac du moi luong ghi vao
//   dia chi khac nhau, nhung neu cac dia chi do nam chung trong mot Cache Line (thuong la 64 bytes),
//   CPU se phai lien tuc invalidate cache line giua cac core (Cache Coherence Ping-Pong).
//   Hien tuong nay goi la False Sharing, gay sut giam nghiem trong hieu nang.
//   Dung padded_u64 dam bao moi phan tu chiem tron 1 Cache Line (64 bytes), triet tieu hoan toan False Sharing.

typedef struct {
    int id;
    uint64_t count;
    uint64_t seed;
    padded_u64 *results;
} thread_arg_t;

void *worker(void *arg) {
    thread_arg_t *tdata = (thread_arg_t *)arg;

    // TODO(B-2): Khoi tao RNG rieng biet cho luong nay bang ham seed_for_thread (Thanh vien B).
    // Goi y:
    //   uint64_t state = seed_for_thread(tdata->seed, tdata->id);
    // Tieu chi: Moi luong co state doc lap, khong chia se state voi luong khac.
    uint64_t state = seed_for_thread(tdata->seed, tdata->id);

    // TODO(B-1): Dung BIEN LOCAL local_inside trong vong lap tinh toan, tuyet doi khong ghi
    // vao mang results trong vong lap. Chi ghi mot lan duy nhat vao results[id].value khi ket thuc (Thanh vien B).
    // Goi y:
    //   uint64_t local_inside = 0;
    //   for (uint64_t i = 0; i < tdata->count; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) local_inside++;
    //   }
    //   tdata->results[tdata->id].value = local_inside;
    // Tieu chi: Vong lap cuc nhanh, khong co chi phi truy cap bo nho chia se hay lock.
    static int warned = 0;
    if (!warned) {
        warned = 1;
        fprintf(stderr, "STUB(B-1, B-2): worker trong pi_local dang chay stub.\n");
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
        fprintf(stderr, "Loi: Khong the cap phat bo nho cho luong.\n");
        exit(EXIT_FAILURE);
    }

    // Do thoi gian tinh toan (bat dau tu luc tao thread den khi join va reduce xong)
    double t_start = now_sec();

    // TODO(B-3): Chia deu khoi luong cong viec bang split_work va tao cac luong pthread_create (Thanh vien B).
    // Goi y:
    //   for (int i = 0; i < cfg.threads; i++) {
    //       args[i].id = i;
    //       args[i].count = split_work(cfg.total, cfg.threads, i);
    //       args[i].seed = cfg.seed;
    //       args[i].results = results;
    //       pthread_create(&threads[i], NULL, worker, &args[i]);
    //   }
    // Tieu chi: Tao dung so luong luong, moi luong nhan du so diem can tinh.
    for (int i = 0; i < cfg.threads; i++) {
        args[i].id = i;
        args[i].count = split_work(cfg.total, cfg.threads, i);
        args[i].seed = cfg.seed;
        args[i].results = results;
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }

    // TODO(B-4): Join tat ca cac luong va thuc hien Reduce (cong don cac results[i].value), sau do tinh pi (Thanh vien B).
    // Goi y:
    //   uint64_t total_inside = 0;
    //   for (int i = 0; i < cfg.threads; i++) {
    //       pthread_join(threads[i], NULL);
    //       total_inside += results[i].value;
    //   }
    //   double pi = 4.0 * (double)total_inside / (double)cfg.total;
    // Tieu chi: Cho day du tat ca luong ket thuc, thu thap ket qua chinh xac va tinh pi xap xi 3.1416.
    uint64_t total_inside = 0;
    for (int i = 0; i < cfg.threads; i++) {
        pthread_join(threads[i], NULL);
        total_inside += results[i].value;
    }

    double pi = 4.0 * (double)total_inside / (double)cfg.total;
    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Giai phong tai nguyen
    free(threads);
    free(args);
    free(results);

    // In dung 1 dong CSV ra stdout
    print_csv("local", cfg.threads, cfg.run, time_sec, pi);

    return 0;
}
