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

Approach 2 implements a multi-threaded Map-Reduce architecture for Monte Carlo estimation of pi.

**Map Phase:** The total workload is distributed among worker threads using `split_work()`. Each thread initializes its private pseudo-random generator using `seed_for_thread()` and accumulates the number of points inside the quarter unit circle in a local variable named `local_inside`.

**Memory Layout:** The results array uses 64-byte `padded_u64` structures.Memory is allocated using `aligned_alloc()` to ensure that each result occupies a separate cache line on processors with 64-byte cache lines. This prevents false sharing between result slots.

**Reduce Phase:** Each worker writes its final count to the results array only once. The main thread waits for all workers using `pthread_join()` and aggregates their partial results.

The estimated value of pi is calculated as:

$$
\hat{\pi} = 4 \times \frac{\sum_{i=0}^{n-1} inside_i}{N}
$$

This approach eliminates the need for mutex locks on the counters inside the computation loop.

The implementation also handles memory allocation failures, thread creation failures, thread joining failures, and zero-point inputs.


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

The scalability of local accumulation depends on the available CPU execution resources.

When the number of threads is below the physical core count, additional threads can improve computational throughput. However, as CPU resources become saturated, further increases in thread count produce diminishing returns.

A preliminary experiment was conducted on a VMware virtual machine with two logical CPUs, using 1,000,000 Monte Carlo points and three repetitions.

| Threads | Mean Time (s) | Speedup |
|---|---:|---:|
| 1 | 0.0037 | 0.86x |
| 2 | 0.0035 | 0.91x |
| 4 | 0.0028 | 1.16x |
| 8 | 0.0026 | 1.24x |
| 16 | 0.0033 | 0.97x |

The lowest execution time was observed at eight threads in this preliminary experiment. Increasing the thread count to sixteen resulted in a higher execution time.

However, these measurements do not establish the actual hardware saturation point because the workload is small, the environment is virtualized, and the common PRNG implementation is not finalized.

The saturation point must be reassessed using the final benchmark results and CPU topology.


### 5.2.C Oversubscription and Context Switching Overhead
*Assigned Member: Member B*

<!-- TODO(B-8): Investigate scenarios where thread counts exceed CPU core capacity (e.g., 32, 64, 128 threads on an 8-core CPU): involuntary context switching, kernel scheduling overhead, and cache thrashing (Member B) -->

Thread oversubscription occurs when the number of runnable threads exceeds the available CPU execution contexts.

The operating system scheduler must share CPU time among competing threads, potentially increasing context switching, scheduling overhead, and cache disruption.

The supporting benchmark script `scripts/b_local_scaling.py` measures voluntary and involuntary context switches using `getrusage()`.

Preliminary experimental results:

| Threads | Mean Involuntary Context Switches |
|---|---:|
| 1 | 3 |
| 2 | 6 |
| 4 | 7 |
| 8 | 12 |
| 16 | 19 |

The number of involuntary context switches increased with the number of threads in this experiment.

At sixteen threads, the execution time increased compared with eight threads, suggesting that scheduling overhead may contribute to performance degradation.

Although local accumulation avoids shared-counter lock contention, excessive thread counts can still introduce operating system overhead.

These observations are preliminary and require verification using the finalized implementation and a larger workload.


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
 
The scalability of Approach 2 can be characterized by three theoretical regimes.

**Regime 1 - Growth:** When the number of threads is below the available physical core count, parallel execution can improve performance by distributing computation across multiple cores.

**Regime 2 - Saturation:** As the available CPU resources become fully utilized, additional threads provide diminishing performance improvements.

**Regime 3 - Oversubscription:** When the number of runnable threads exceeds the available logical CPUs, context switching and scheduling overhead may degrade performance.

Speedup is calculated as:

$$
S(n) = \frac{T_{single}}{T_n}
$$

Parallel efficiency is calculated as:

$$
E(n) = \frac{S(n)}{n}
$$

The preliminary experiment produced the following results:

| Threads | Speedup | Parallel Efficiency |
|---|---:|---:|
| 1 | 0.86x | 86.4% |
| 2 | 0.91x | 45.6% |
| 4 | 1.16x | 29.0% |
| 8 | 1.24x | 15.5% |
| 16 | 0.97x | 6.0% |

Parallel efficiency decreased as the number of threads increased. The preliminary results demonstrate that increasing the number of threads does not necessarily improve performance.

#### Supporting False Sharing Experiment

The file `src/experiments/false_sharing_demo.c` compares three counter placement strategies.

| Counter Placement | 1 Thread (s) | 2 Threads (s) | Speedup |
|---|---:|---:|---:|
| Local | 0.0058 | 0.0040 | 1.42x |
| Padded | 0.0058 | 0.0054 | 1.06x |
| Unpadded | 0.0055 | 0.0051 | 1.09x |

Local accumulation achieved the highest speedup in this preliminary experiment.

However, the padded variant did not outperform the unpadded variant. Therefore, these measurements are insufficient to establish a clear false sharing penalty.

Further experiments with larger workloads and the finalized common implementation are required.


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
