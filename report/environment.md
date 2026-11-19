# Experimental Environment

This document records the hardware specifications, operating system, and compiler configuration of the Linux VM used to run the final benchmarks (`scripts/run_bench.sh` will automatically append `uname -a`, `lscpu`, and `gcc --version` when executed).

## 1. Hardware Specifications & CPU Topology
- **CPU Model**: *(To be filled from `lscpu` on Linux VM)*
- **Physical Cores / Logical Cores (SMT)**: *(To be filled from `lscpu` on Linux VM)*
- **L1 / L2 / L3 Cache Hierarchy**: *(To be filled from `lscpu` on Linux VM)*
- **System Memory (RAM)**: *(To be filled from `free -h` on Linux VM)*

## 2. Operating System
- **OS Platform & Version**: *(To be filled from `/etc/os-release` or `uname -a` on Linux VM)*
- **Kernel Version**: *(To be filled from `uname -r` on Linux VM)*

## 3. Compiler & Build Flags
- **C Compiler**: `gcc` (C11, POSIX threads)
- **Compiler Optimization Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`
