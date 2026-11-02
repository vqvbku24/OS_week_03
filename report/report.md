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

<!-- TODO(C-4): Present Shared Counter design: single global shared_inside counter, pthread_mutex_lock/unlock around every circle hit, structural performance bottleneck (Member C) -->

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

<!-- TODO(C-5): Analyze why Approach 3 is orders of magnitude slower than Approach 2: thread serialization, sleep/wake queue penalties, and degradation of parallel execution into penalized sequential execution (Member C) -->

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

<!-- TODO(C-6): Provide comprehensive head-to-head empirical comparison between Share-Nothing (Local state) and Shared-State (Mutex-locked) architectures (Member C) -->

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
