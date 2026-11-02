/**
 * pi_single.c - Approach 1: Single thread baseline
 * Nguoi phu trach: QVinh (Thanh vien A)
 */

#include "common.h"

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    // Bat dau do thoi gian tinh toan (khong do phan parse_args hay print)
    double t_start = now_sec();

    uint64_t inside = 0;
    uint64_t state = cfg.seed;
    if (state == 0) state = 12345;

    // TODO(A-6): Vong lap sinh total diem (x,y) trong [0,1], dem so diem roi vao hinh tron (QVinh - Thanh vien A).
    // Goi y:
    //   for (uint64_t i = 0; i < cfg.total; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) inside++;
    //   }
    // Tieu chi: Chay dung cfg.total diem va cap nhat chinh xac bien inside.
    fprintf(stderr, "STUB(A-6): pi_single dang chay stub Monte Carlo.\n");
    for (uint64_t i = 0; i < cfg.total; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    // TODO(A-7): Tinh toan uoc luong gia tri pi = 4.0 * inside / total va kiem tra pi xap xi 3.1416 (QVinh - Thanh vien A).
    // Goi y:
    //   double pi = 4.0 * (double)inside / (double)cfg.total;
    // Tieu chi: Voi total lon (vd: 1e7 - 1e8), gia tri pi uoc luong phai xap xi 3.14159.
    double pi = 4.0 * (double)inside / (double)cfg.total;

    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // In dung 1 dong CSV ra stdout theo hop dong
    print_csv("single", 1, cfg.run, time_sec, pi);

    return 0;
}
