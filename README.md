# Lab: Estimating $\pi$ via Multi-Threaded Monte Carlo

Operating Systems laboratory project investigating, implementing, and evaluating multi-threaded concurrency models through Monte Carlo approximation of $\pi$.

---

## 1. Group Member Assignments

| Member | Code | Primary Responsibilities | Dedicated Git Branch |
|---|---|---|---|
| **QVinh** | **A** | Core platform architecture (`src/common.h`), Approach 1 (`src/pi_single.c`), measurement methodology | `feat/A-common` |
| Member 2 | **B** | Approach 2 Map-Reduce (`src/pi_local.c`), thread scaling, memory padding | `feat/B-local` |
| Member 3 | **C** | Approach 3 Mutex Lock (`src/pi_mutex.c`), lock contention analysis | `feat/C-mutex` |
| Member 4 | **D** | Mutex batching variant (`src/pi_mutex_k.c`), lock-free atomic (`src/pi_atomic.c`) | `feat/D-mutexk-atomic` |
| Member 5 | **E** | Automated benchmark runner (`scripts/run_bench.sh`), plotting (`scripts/plot.py`), report synthesis | `feat/E-bench-plot` |

> Detailed work breakdown, completion criteria, and specific task checklist `TODO(X-n)` are maintained in [`TASKS.md`](TASKS.md).

---

## 2. Directory Layout

```text
Week-03/ (pi-montecarlo-lab)
├── Makefile                # Build automation rules (all, check, bench, tradeoff, plot, clean)
├── README.md               # Project guide and overview
├── TASKS.md                # Task management and member checklist
├── .gitignore              # Ignores build binaries and output data
├── src/                    # C source files (C11, pthread)
│   ├── common.h            # Shared header: argument parser, high-res timer, PRNG, structures (A)
│   ├── pi_single.c         # Approach 1: Single thread baseline (A)
│   ├── pi_local.c          # Approach 2: Multi-thread Local Accumulation (B)
│   ├── pi_mutex.c          # Approach 3: Shared Counter with Mutex Lock (C)
│   ├── pi_mutex_k.c        # Experiment 6.1: Mutex batching with interval k (D)
│   └── pi_atomic.c         # Multi-thread lock-free atomic fetch-add (D)
├── scripts/                # Benchmark automation and visualization
│   ├── run_bench.sh        # Full benchmark runner across approaches and threads (E)
│   ├── run_tradeoff.sh     # Trade-off experiments runner (6.1 & 6.3) (E)
│   └── plot.py             # Data processing and chart generation (pandas, matplotlib) (E)
├── results/                # Output directory for CSV records
├── plots/                  # Output directory for exported figures (.png)
└── report/                 # Laboratory documentation
    ├── report.md           # Group lab report
    └── environment.md      # Hardware, OS, and toolchain specification
```

---

## 3. Technical Contract

### 3.1 Unified Command-Line Interface
All executables in `src/` strictly adhere to the following signature:
```bash
./prog <total_points> <num_threads> <seed> <run_id> [k]
```
- `total_points`: Total Monte Carlo points to evaluate (default: $10^8 = 100000000$).
- `num_threads`: Number of worker threads ($1, 2, 4, 8, 16, \dots$). `pi_single` ignores this value.
- `seed`: Base random seed for PRNG initialization.
- `run_id`: Execution sequence identifier ($1, 2, 3, \dots$).
- `k`: Batching frequency (applied solely to `pi_mutex_k`, default: $1000$).

### 3.2 Output Format (Standard CSV)
Each execution prints **EXACTLY ONE LINE** to `stdout`:
```csv
approach,threads,run,time_sec,pi_estimate
```
*Example:* `local,4,1,0.185240,3.141582000`

---

## 4. Usage Guide

### 4.1 Compile Everything
```bash
make
# Or on Windows using MinGW: mingw32-make
```
All 5 binaries will be built into the `bin/` directory.

### 4.2 Run Quick Smoke Test
```bash
make check
```
Quickly runs all 5 programs with $N = 10^6$ points to ensure valid execution and verify $\pi \approx 3.1416$.

### 4.3 Run Full Benchmark Suite
```bash
make bench
```
Invokes `scripts/run_bench.sh`, populating `results/results.csv` and logging hardware details into `report/environment.md`.

### 4.4 Run Trade-off Experiments
```bash
make tradeoff
```
Runs `scripts/run_tradeoff.sh` to evaluate batching frequency $k$.

### 4.5 Generate Analytical Plots
```bash
make plot
```
Reads data from `results/`, calculates statistical metrics (mean, std dev, speedup, efficiency), and outputs PNG figures to `plots/`.

---

## 5. Git Workflow & Collaboration

1. **Feature Branches**: Every member develops on their designated branch:
   - QVinh: `git checkout feat/A-common`
   - Member B: `git checkout feat/B-local`
   - Member C: `git checkout feat/C-mutex`
   - Member D: `git checkout feat/D-mutexk-atomic`
   - Member E: `git checkout feat/E-bench-plot`
2. **Conventional Commits**: Use descriptive commit prefixes (e.g., `feat(common): implement xorshift64 PRNG`).
3. **Pull Requests & Code Reviews**: Merge into `main` exclusively through Pull Requests requiring at least one peer approval.
