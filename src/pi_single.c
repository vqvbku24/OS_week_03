/**
 * pi_single.c - Approach 1: Single thread baseline
 * Assigned Member: QVinh (Member A)
 */

#include "common.h"

int main(int argc, char **argv) {
    config_t cfg = parse_args(argc, argv);

    // Measure computational workload time only (exclude argument parsing and printing)
    double t_start = now_sec();

    uint64_t inside = 0;
    uint64_t state = seed_for_thread(cfg.seed, 0);

    for (uint64_t i = 0; i < cfg.total; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    double pi = (cfg.total > 0) ? (4.0 * (double)inside / (double)cfg.total) : 0.0;

    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Print exactly ONE CSV row conforming to contract
    print_csv("single", 1, cfg.run, time_sec, pi);

    return 0;
}
