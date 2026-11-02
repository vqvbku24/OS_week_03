# Experimental Environment

This document records the hardware specifications, operating system, and compiler configuration used to benchmark all multi-threading approaches (maintained by **Member E** via `scripts/run_bench.sh`).

## 1. Hardware Specifications & CPU Topology
<!-- TODO(E-2): scripts/run_bench.sh will automatically append lscpu / uname -a output here -->
- **CPU Model**: Updated during formal benchmark run
- **Physical Cores / Logical Cores (SMT)**: 
- **L1 / L2 / L3 Cache Hierarchies**: 
- **System Memory (RAM)**: 

## 2. Operating System
- **OS Platform & Version**: 
- **Kernel Version**: 

## 3. Compiler & Build Flags
- **C Compiler**: gcc (supporting C11 standard, POSIX threads)
- **Compiler Optimization Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`
