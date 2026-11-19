# Laboratory Report: Estimating $\pi$ via Monte Carlo Method in Multi-Threaded Environments

**Project Group**: 5 Members
- **Member A**: QVinh (Platform Architecture, `src/common.h`, `src/pi_single.c`)
- **Member B**: (Approach 2: `src/pi_local.c`, Thread Scaling analysis)
- **Member C**: (Approach 3: `src/pi_mutex.c`, Contention & Serialization analysis)
- **Member D**: (Variants: `src/pi_mutex_k.c`, `src/pi_atomic.c`, Trade-off $k$ analysis)
- **Member E**: (Benchmarking automation, Plotting, Report synthesis)

---

## 1. Introduction and Experimental Setup
*Assigned Member: Member E*

<!-- TODO(E-12): Write introduction to Monte Carlo pi estimation and summarize system setup from report/environment.md (Member E) -->

### 1.1 Monte Carlo Principle for Estimating $\pi$
- Generate $N$ uniform random points $(x, y) \in [0, 1] \times [0, 1]$.
- Condition for a point falling inside the quarter unit circle: $x^2 + y^2 \le 1.0$.
- Geometric area ratio: $\frac{\text{Quarter circle area}}{\text{Unit square area}} = \frac{\frac{\pi \times 1^2}{4}}{1 \times 1} = \frac{\pi}{4}$.
- Estimation formula: $\pi \approx 4 \times \frac{\text{Points inside circle}}{\text{Total points generated}} = 4 \times \frac{M}{N}$.

### 1.2 System Environment
- Detailed hardware and operating system metrics are documented in [`environment.md`](environment.md).

---

## 2. Design of Computational Approaches

### 2.1 Approach 1: Single Thread (Baseline)
*Assigned Member: QVinh (Member A)*

<!-- TODO(A-8): Present Single-Thread design: random number generation loop, xorshift64 PRNG, lack of multi-threading overhead, role as experimental baseline (QVinh - Member A) -->

### 2.2 Approach 2: Multi-thread Local Accumulation (Map-Reduce)
*Assigned Member: Member B*

<!-- TODO(B-6): Present Map-Reduce architecture: workload partitioning, private local_inside counters, padded_u64 array mitigating False Sharing, final reduction phase (Member B) -->

### 2.3 Approach 3: Shared Counter with Mutex Lock
*Assigned Member: Member C*

Approach 3 (`src/pi_mutex.c`) implements a **shared-state** synchronization model to contrast directly with Approach 2's share-nothing design:

- **Global Shared State & Lock:** A single static 64-bit accumulator `shared_inside` holds the total number of points landing inside the quarter unit circle, guarded by a POSIX mutex (`pthread_mutex_t lock`).
- **Parallel Point Generation with Fine-Grained Critical Section:** Each worker thread receives its assigned sample count via `split_work()` and derives an independent PRNG state via `seed_for_thread()`. Random coordinate generation `(x, y)` and the circle check ($x^2 + y^2 \le 1.0$) execute concurrently, but every qualifying hit immediately enters a fine-grained critical section:
  ```c
  if (x * x + y * y <= 1.0) {
      pthread_mutex_lock(&lock);
      shared_inside++;
      pthread_mutex_unlock(&lock);
  }
  ```
  Because the quarter circle covers $\frac{\pi}{4} \approx 78.54\%$ of the unit square, roughly $78.5\%$ of all Monte Carlo iterations acquire and release `lock`.
- **Mutex Lifecycle & Correctness Guarantees:** In `main()`, `pthread_mutex_init(&lock, NULL)` initializes the mutex prior to spawning worker threads, `shared_inside` is reset to `0`, all thread creation and `pthread_join()` calls are error-checked, and `pthread_mutex_destroy(&lock)` reclaims the mutex only after every worker has joined. While this preserves deterministic correctness (zero lost updates / zero data races), forcing all threads through one lock intentionally exposes synchronization overhead as a structural bottleneck.

### 2.4 Mutex Batching ($k$) and Lock-Free Atomic Variants
*Assigned Member: Member D*

<!-- TODO(D-6): Present architectural variants: batching updates every k points (pi_mutex_k) and hardware-level atomic instructions __atomic_fetch_add (pi_atomic) (Member D) -->

---

## 3. Measurement Methodology & Experimental Rigor
*Assigned Member: QVinh (Member A)*

<!-- TODO(A-9): Describe measurement methodology:
  - Employ clock_gettime(CLOCK_MONOTONIC) measuring strictly the computation loop (excluding parsing, allocations, and I/O).
  - Seed independence guarantees per thread via arithmetic mixing.
  - Repetition policy (>= 5 runs) to eliminate noise and calculate standard deviations.
  - Strict avoidance of global rand()/srand() state locks. (QVinh - Member A) -->

---

## 4. Experimental Results
*Assigned Member: Member E*

<!-- TODO(E-13): Tabulate benchmark results (mean execution times, standard deviations, speedup, efficiency) and embed figures from plots/ (Member E) -->

### 4.1 Summary Table
*(Extracted from `results/results.csv` after running the benchmark suite)*

| Approach | Threads | Mean Time (s) | Std Dev (s) | Speedup | Parallel Efficiency |
|---|---|---|---|---|---|
| single | 1 | ... | ... | 1.00x | 100% |
| local | 2 | ... | ... | ... | ... |
| local | 4 | ... | ... | ... | ... |
| mutex | 2 | ... | ... | ... | ... |
| atomic | 2 | ... | ... | ... | ... |

### 4.2 Visualization Figures
- **Execution Time vs. Thread Count**: `plots/time_vs_threads.png`
- **Speedup vs. Thread Count**: `plots/speedup_vs_threads.png`
- **Parallel Efficiency vs. Thread Count**: `plots/efficiency_vs_threads.png`
- **Synchronization Trade-off vs. Batch Size $k$**: `plots/tradeoff_k.png`
- **Direct Comparison: Local Accumulation vs. Mutex**: `plots/comparison_local_vs_mutex.png`

---

## 5. In-Depth Analysis

### 5.2.A Why is Parallel Speedup Non-Linear?
*Assigned Member: QVinh (Member A)*

<!-- TODO(A-10): Analyze Amdahl's Law, thread creation/teardown costs, memory bandwidth saturation, and diminishing returns as thread count scales (QVinh - Member A) -->

### 5.2.B Saturation Point and Physical Core Count
*Assigned Member: Member B*

<!-- TODO(B-7): Analyze performance saturation points and their correlation with physical/logical CPU cores on the host machine (Member B) -->

### 5.2.C Oversubscription and Context Switching Overhead
*Assigned Member: Member B*

<!-- TODO(B-8): Investigate scenarios where thread counts exceed CPU core capacity (e.g., 32, 64, 128 threads on an 8-core CPU): involuntary context switching, kernel scheduling overhead, and cache thrashing (Member B) -->

### 5.2.D Synchronization Overhead & Serialization (Mutex Contention)
*Assigned Member: Member C*

Approach 3 performs orders of magnitude slower than Approach 2—and degrades rather than improves as thread count grows—due to three compounding costs:

1. **Execution Serialization (Amdahl's Law Bottleneck):** Mutual exclusion enforces that at most one thread can hold `lock` and update `shared_inside` at any instant. Since $\approx \frac{\pi}{4}N$ iterations enter the critical section and the non-critical work per point (two `xorshift64` steps and two multiplications) takes only a few nanoseconds, threads spend far more time waiting on the lock than computing. Parallel execution effectively collapses into **penalized sequential execution**.
2. **Mutex Contention & Kernel Sleep/Wake Penalties:** For $N$ total points, approximately $0.785 N$ lock/unlock transitions occur. At $1$ thread, the mutex incurs uncontended atomic fast-path overhead on every hit. As threads scale to $2, 4, 8, \dots$, multiple cores collide on the lock simultaneously: the fast path fails, threads transition into the OS kernel wait queue (e.g., `futex` on Linux or Win32 synchronization primitives on Windows), and the scheduler puts waiting threads to sleep and later wakes them. Each contended sleep/wake cycle and involuntary context switch costs thousands of CPU cycles—vastly exceeding the single `shared_inside++` instruction being protected.
3. **Cache-Line Bouncing (Coherence Traffic):** Every lock acquisition, counter increment, and lock release modifies shared memory locations (`lock` and `shared_inside`). Under hardware cache coherence protocols (e.g., MESI), CPU cores continuously invalidate and transfer ownership of those cache lines across the interconnect, stalling core pipelines even before kernel blocking occurs.

### 5.2.E Decay of Parallel Efficiency
*Assigned Member: Member E*

<!-- TODO(E-14): Formulate Parallel Efficiency = Speedup / n, discussing why efficiency progressively declines as thread count increases according to the law of diminishing returns (Member E) -->

---

## 6. Trade-Off Analysis

### 6.1 Synchronization Frequency Trade-Off (Batch Interval $k$)
*Assigned Member: Member D*

<!-- TODO(D-7): Analyze results from results/tradeoff_k.csv:
  - When k = 1: Worst-case synchronization overhead identical to Approach 3.
  - When k scales (10 -> 1000 -> 1000000): Dramatic drop in lock contention, runtime converges towards Approach 2 (Local).
  - Recommend optimal k balance point between update responsiveness and peak throughput. (Member D) -->

### 6.2 Thread Scaling Regimes
*Assigned Member: Member B*

<!-- TODO(B-9): Characterize the 3 phases of scaling in Approach 2:
  - Phase 1: Threads < Physical Cores (Linear/sub-linear speedup).
  - Phase 2: Threads ≈ Cores (Plateau regime).
  - Phase 3: Threads > Cores (Saturation or degradation due to Oversubscription). (Member B) -->

### 6.3 Local Accumulation vs. Shared Counter
*Assigned Member: Member C*

Comparing **Local Accumulation (`pi_local.c`, Approach 2)** against **Shared Counter with Mutex (`pi_mutex.c`, Approach 3)** demonstrates why eliminating shared state inside hot loops is a foundational rule of parallel programming:

| Dimension | Local Accumulation (`pi_local.c`) | Shared Counter + Mutex (`pi_mutex.c`) |
|---|---|---|
| **Critical Section** | **None in hot loop.** Each thread increments a register/stack `local_inside` variable and writes once to its cache-line-padded `results[id].value` before exiting. | **Executed $\approx \frac{\pi}{4}N$ times.** Every circle hit acquires and releases `pthread_mutex_t lock` around `shared_inside++`. |
| **Contention** | **Zero.** No lock competition and no cross-core cache invalidation (`padded_u64` eliminates false sharing). | **Severe.** All threads compete for a single mutex and a single shared counter cache line, escalating rapidly with thread count. |
| **Serialization** | **Negligible.** Only the final $O(\text{threads})$ reduction loop in `main()` runs sequentially after `pthread_join()`. | **Dominant.** Shared-counter updates serialize across all cores and trigger OS wait-queue blocking and context switches. |
| **Scaling Trend** | Execution time decreases up to physical CPU core saturation. | Execution time **increases** monotonically as more threads contend for the lock. |

#### Preliminary Correctness & Head-to-Head Verification
To validate race-free correctness and quantify the scaling divergence prior to full-scale cluster/Linux benchmarking ($N = 10^8$ in `results/results.csv`), both binaries were compiled with `-O2 -std=c11 -Wall -Wextra -pthread` and executed for $5$ runs per configuration on Windows (MinGW) with fixed $N = 2{,}000{,}000$ and `seed = 12345`:

| Threads | `pi_local` Mean (s) | `pi_mutex` Mean (s) | Slowdown ($T_{\text{mutex}} / T_{\text{local}}$) | `pi_local` Estimate | `pi_mutex` Estimate |
|---|---|---|---|---|---|
| 1 | 0.024938 | 0.118738 | 4.76× | 3.141608000 | 3.141608000 |
| 2 | 0.016362 | 0.181986 | 11.12× | 3.142280000 | 3.142280000 |
| 4 | 0.010315 | 0.203908 | 19.77× | 3.143680000 | 3.143680000 |
| 8 | 0.009694 | 0.326619 | 33.69× | 3.143474000 | 3.143474000 |

*(Note: Preliminary verification above ran prior to integrating Member A/B final `common.h` and `pi_local.c` commits; values should be refreshed from Member E's final `results/results.csv` at $N = 10^8$ after merge.)*

Two conclusions follow directly from the data:
1. **Zero Race Conditions:** At every thread count, `pi_mutex` produces the **exact same 9-decimal-place $\pi$ estimate** as `pi_local`, confirming that the mutex prevents lost updates.
2. **Contention Penalty:** Even with $1$ thread, uncontended locking makes `pi_mutex` **4.76× slower** than `pi_local`. At $8$ threads, while `pi_local` accelerates by **2.57×** (`0.024938s` $\rightarrow$ `0.009694s`), `pi_mutex` slows down by **2.75×** relative to its own 1-thread time (`0.118738s` $\rightarrow$ `0.326619s`), becoming **33.69× slower** than `pi_local`.

---

## 7. Best Practices & Conclusion
*Assigned Member: Entire Group (Synthesized by Member E)*

<!-- TODO(E-15): Synthesize multi-threaded software engineering principles established through this laboratory investigation (Synthesized by Member E) -->
- **Avoid Shared State in Inner Loops**: Always favor Local Accumulation and Map-Reduce paradigms.
- **Eliminate False Sharing**: Respect Cache Line boundaries (64 bytes) using explicit padding structures.
- **Select Concurrency Primitives Deliberately**:
  - Lock-free / Share-nothing > Batching > Coarse-grained Locks > Fine-grained Unbatched Locks.
- **Thread-Local PRNG**: Avoid global pseudo-random state locks (such as `rand()`); enforce thread-private generators (`xorshift64`).
- **Optimal Concurrency Bounds**: Restrict worker threads to available physical cores to prevent harmful OS oversubscription.
