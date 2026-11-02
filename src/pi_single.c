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
    uint64_t state = cfg.seed;
    if (state == 0) state = 12345;

    // TODO(A-6): Loop to generate total points (x,y) in [0,1], count points falling inside the unit circle quarter (QVinh - Member A).
    // Hint:
    //   for (uint64_t i = 0; i < cfg.total; i++) {
    //       double x = rand01(&state);
    //       double y = rand01(&state);
    //       if (x * x + y * y <= 1.0) inside++;
    //   }
    // Acceptance criteria: Evaluates cfg.total points and updates inside count accurately.
    fprintf(stderr, "STUB(A-6): pi_single is running Monte Carlo loop.\n");
    for (uint64_t i = 0; i < cfg.total; i++) {
        double x = rand01(&state);
        double y = rand01(&state);
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    // TODO(A-7): Calculate estimated pi = 4.0 * inside / total and verify pi approaches 3.1416 (QVinh - Member A).
    // Hint:
    //   double pi = 4.0 * (double)inside / (double)cfg.total;
    // Acceptance criteria: For large total (e.g. 1e7 - 1e8), estimated pi should be close to 3.14159.
    double pi = 4.0 * (double)inside / (double)cfg.total;

    double t_end = now_sec();
    double time_sec = t_end - t_start;

    // Print exactly ONE CSV row conforming to contract
    print_csv("single", 1, cfg.run, time_sec, pi);

    return 0;
}
