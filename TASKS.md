# TASKS: Group Work Breakdown (Monte Carlo π Lab)

General Convention: `TODO(X-n)` corresponds to unique task identifiers located across the codebase and report documents. Members tick `[x]` upon completion and verification (after merging into `main`).

---

## Member A: QVinh — Common Foundation & Approach 1 (Single Thread)

**Role**: Group Lead / Platform Architect. Delivers `src/common.h` (the core technical contract) and the single-threaded baseline. **Target deadline: Early Days 1–2.**

**Assigned Files**: `src/common.h`, `src/pi_single.c`, assigned sections in `report/report.md`.

**Checklist**
- [ ] TODO(A-1) `src/common.h`: Implement `now_sec()` using `clock_gettime(CLOCK_MONOTONIC)`.
- [ ] TODO(A-2) `src/common.h`: Implement standard `xorshift64()` (13, 7, 17); handle `state != 0`.
- [ ] TODO(A-3) `src/common.h`: Implement `rand01()` converting 64-bit integer to `double` in `[0, 1)`.
- [ ] TODO(A-4) `src/common.h`: Implement `split_work()` distributing work evenly, assigning remainder to last thread.
- [ ] TODO(A-5) `src/common.h`: Implement `seed_for_thread()` producing independent, mixed seeds per thread.
- [ ] TODO(A-6) `src/pi_single.c`: Monte Carlo loop generating points and counting circle hits.
- [ ] TODO(A-7) `src/pi_single.c`: Compute $\pi = 4.0 \times \frac{\text{inside}}{\text{total}}$, verifying $\pi \approx 3.1416$.
- [ ] TODO(A-8) `report/report.md`: Write Section 2.1 — Approach 1 Design (Single thread baseline).
- [ ] TODO(A-9) `report/report.md`: Write Section 3 — Measurement Methodology (monotonic clock, seed independence, $\ge 5$ runs).
- [ ] TODO(A-10) `report/report.md`: Write Section 5.2.A — Non-linear Speedup Analysis (Amdahl's law, thread overhead, diminishing returns).

**Dependencies**: None.
**Definition of Done (DoD)**:
- Running `./bin/pi_single 1000000 1 12345 1` yields $\pi \approx 3.1416$.
- `src/common.h` is fully stable and merged into `main` for Members B, C, and D.

---

## Member B: Approach 2 (Local Accumulation / Map-Reduce)

**Role**: High-performance multi-threading engineer; evaluates thread scaling and memory padding.

**Assigned Files**: `src/pi_local.c`, assigned sections in `report/report.md`.

**Checklist**
- [ ] TODO(B-1) `src/pi_local.c`: Worker accumulates into local variable `local_inside`, writing to `results[id]` only at completion.
- [ ] TODO(B-2) `src/pi_local.c`: Initialize independent PRNG state per thread via `seed_for_thread`.
- [ ] TODO(B-3) `src/pi_local.c`: Partition workload evenly via `split_work` and invoke `pthread_create`.
- [ ] TODO(B-4) `src/pi_local.c`: Synchronize threads (`pthread_join`), perform final Reduce phase, and estimate $\pi$.
- [ ] TODO(B-5) `src/pi_local.c`: Document rationale for `padded_u64` Cache Line alignment preventing False Sharing.
- [ ] TODO(B-6) `report/report.md`: Write Section 2.2 — Approach 2 Design (Map phase, Reduce phase, memory layout).
- [ ] TODO(B-7) `report/report.md`: Write Section 5.2.B — Saturation Point Analysis & CPU core mapping.
- [ ] TODO(B-8) `report/report.md`: Write Section 5.2.C — Thread Oversubscription & Context Switching penalties.
- [ ] TODO(B-9) `report/report.md`: Write Section 6.2 — Thread Scaling Analysis across the 3 regimes.

**Dependencies**: `src/common.h` by QVinh (Member A).
**Definition of Done (DoD)**:
- Correct $\pi$ estimate across all thread counts ($1, 2, 4, 8, \dots$).
- Single-thread execution matches `pi_single` wall-clock time.
- Speedup scales cleanly while thread count $\le$ physical core count.

---

## Member C: Approach 3 (Shared Counter with Mutex Lock)

**Role**: Concurrency bottleneck engineer; demonstrates synchronization overhead and lock contention.

**Assigned Files**: `src/pi_mutex.c`, assigned sections in `report/report.md`.

**Checklist**
- [ ] TODO(C-1) `src/pi_mutex.c`: Wrap `pthread_mutex_lock/unlock` around every increment of global `shared_inside` in the hot loop.
- [ ] TODO(C-2) `src/pi_mutex.c`: Properly initialize and destroy mutex (`pthread_mutex_init`, `pthread_mutex_destroy`).
- [ ] TODO(C-3) `src/pi_mutex.c`: Verify correctness (no race condition) by matching results with `pi_local` under identical seeds.
- [ ] TODO(C-4) `report/report.md`: Write Section 2.3 — Approach 3 Design (Shared counter, locking mechanics).
- [ ] TODO(C-5) `report/report.md`: Write Section 5.2.D — Mutex Contention & Serialization Analysis.
- [ ] TODO(C-6) `report/report.md`: Write Section 6.3 — Direct Comparison: Local Accumulation vs. Shared Counter.

**Dependencies**: `src/common.h` by QVinh (Member A); benchmark numbers from Member B for comparative analysis.
**Definition of Done (DoD)**:
- Correct estimation of $\pi$ without race conditions.
- Measurable and significant slowdown compared to `pi_local` as thread counts scale.

---

## Member D: Synchronization Trade-offs (Batching Mutex & Lock-free Atomic)

**Role**: Concurrency optimization researcher; evaluates batching thresholds ($k$) and atomic hardware instructions.

**Assigned Files**: `src/pi_mutex_k.c`, `src/pi_atomic.c`, assigned sections in `report/report.md`.

**Checklist**
- [ ] TODO(D-1) `src/pi_mutex_k.c`: Accumulate locally and batch flush to `shared_inside` every $k$ hits.
- [ ] TODO(D-2) `src/pi_mutex_k.c`: Correctly flush leftover hit counts upon loop exit.
- [ ] TODO(D-3) `src/pi_mutex_k.c`: Verify asymptotic properties: $k = 1 \approx$ `pi_mutex`, large $k \approx$ `pi_local`.
- [ ] TODO(D-4) `src/pi_atomic.c`: Implement `__atomic_fetch_add(&shared_inside, 1, __ATOMIC_RELAXED)`.
- [ ] TODO(D-5) `src/pi_atomic.c`: Document comparative analysis between Mutex vs. Atomic vs. Local in file header.
- [ ] TODO(D-6) `report/report.md`: Write Section 2.4 — Batching Mutex ($k$) & Atomic Primitives Design.
- [ ] TODO(D-7) `report/report.md`: Write Section 6.1 — Synchronization Frequency Trade-Off Analysis.

**Dependencies**: `src/common.h` by QVinh (Member A) and mutex pattern from Member C.
**Definition of Done (DoD)**:
- Accurate $\pi$ estimation across all values of $k$ ($1, 10, 100, 1000, \dots$) and in `pi_atomic`.
- Data reflects execution time decreasing logarithmically as $k$ increases.

---

## Member E: Benchmarking Automation, Visualization & Report Synthesis

**Role**: Performance automation engineer & technical writer; builds the evaluation pipeline and synthesizes findings.

**Assigned Files**: `scripts/run_bench.sh`, `scripts/run_tradeoff.sh`, `scripts/plot.py`, `report/environment.md`, `report/report.md`.

**Checklist**
- [ ] TODO(E-1) `scripts/run_bench.sh`: Automate runs across approach $\times$ threads $\times 5$ repetitions, writing standard CSV rows.
- [ ] TODO(E-2) `scripts/run_bench.sh`: Collect hardware topology (`lscpu`, `uname -a`) into `report/environment.md`.
- [ ] TODO(E-3) `scripts/run_bench.sh`: Support parameterization ($N$, thread lists, run counts) via environment variables.
- [ ] TODO(E-4) `scripts/run_bench.sh`: Execute unrecorded warm-up run to eliminate cold-start cache anomalies.
- [ ] TODO(E-5) `scripts/run_tradeoff.sh`: Automate Experiment 6.1 over $k$ values, saving output to `results/tradeoff_k.csv`.
- [ ] TODO(E-6) `scripts/run_tradeoff.sh`: Reuse benchmark records for Experiment 6.3 without redundant execution.
- [ ] TODO(E-7) `scripts/plot.py`: Parse CSV data, compute mean execution times and standard deviations.
- [ ] TODO(E-8) `scripts/plot.py`: Calculate Speedup ($T_{\text{single}} / T_n$) and Parallel Efficiency ($\text{Speedup} / n$).
- [ ] TODO(E-9) `scripts/plot.py`: Generate and save 3 core plots (time, speedup, efficiency) and overall comparison to `plots/`.
- [ ] TODO(E-10) `scripts/plot.py`: Generate Experiment 6.1 plot (time vs. $k$ on log scale).
- [ ] TODO(E-11) `scripts/plot.py`: Draw vertical reference line indicating physical CPU core count on all graphs.
- [ ] TODO(E-12) `report/report.md`: Write Section 1 — Introduction and Experimental Setup.
- [ ] TODO(E-13) `report/report.md`: Present Section 4 — Result Tables and Figure Embeddings from `plots/`.
- [ ] TODO(E-14) `report/report.md`: Write Section 5.2.E — Parallel Efficiency Degradation Analysis.
- [ ] TODO(E-15) `report/report.md`: Write Section 7 — Final Conclusion and Synthesis of Group Best Practices.

**Dependencies**: Requires functional binaries from A, B, C, D for formal data collection.
**Definition of Done (DoD)**:
- Running `make bench && make plot` runs unattended to generate all CSV data files and PNG figures.

---

## 7-Day Project Timeline

| Day | Milestone |
|---|---|
| **Day 1** | Kickoff meeting. Review contract. Members branch from `main`. **QVinh (A)** merges `src/common.h` |
| **Day 2** | **QVinh (A)** completes `pi_single.c`. **B, C, D** implement core algorithms. **E** tests pipeline with mock data |
| **Day 3** | Code freeze. Verify $\pi \approx 3.1416$ via `make check`. Execute **Peer Review Matrix** |
| **Day 4–5** | **E** runs formal benchmark suite on a single dedicated machine with minimal background processes |
| **Day 5–6** | Each member drafts their assigned analytical sections in `report/report.md` based on real data |
| **Day 7** | Comprehensive editorial pass, proofreading, packaging source code, and submission |

## Peer Review Matrix (Day 3)

- **QVinh (A)** reviews **B**: Verify remainder handling in `split_work` and cache padding `padded_u64`.
- **B** reviews **C**: Inspect Critical Section scope, proper initialization/destruction of mutex.
- **C** reviews **D**: Verify residual hit flush logic in `pi_mutex_k.c` and atomic memory ordering in `pi_atomic.c`.
- **D** reviews **E**: Review bash benchmark scripts, argument parsing, and speedup calculations in `plot.py`.
- **E** reviews **QVinh (A)**: Verify `common.h` contracts, `rand01` uniformity, and strict CSV formatting.

---

## Final Submission Checklist

- [ ] All 5 C programs compile cleanly without warnings (`make` succeeds with 0 warnings).
- [ ] Raw data files produced: `results/results.csv` and `results/tradeoff_k.csv`.
- [ ] All visualization charts generated in `plots/` (`time`, `speedup`, `efficiency`, `tradeoff_k`, `comparison`).
- [ ] Comprehensive laboratory report `report/report.md` with all 7 sections fully written and formatted.
- [ ] System environment file `report/environment.md` correctly documenting experimental testbed.
