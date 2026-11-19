# Experimental Environment

This document records the hardware specifications, operating system, and compiler configuration used for the Monte Carlo $\pi$ multi-threading benchmarks (maintained by **Member E**; `scripts/run_bench.sh` also updates this file automatically when running on Linux).

## 1. Hardware Specifications & CPU Topology
- **CPU Model**: Intel(R) Core(TM) Ultra 5 225U
- **Physical Cores / Logical Cores (SMT)**: 12 physical cores / 14 logical processors
- **Cache Hierarchy**: L2 Cache 10 MB (10,240 KB), L3 Cache 12 MB (12,288 KB)
- **System Memory (RAM)**: ~15 GB usable (16 GB installed, `15642428` KB visible)

## 2. Operating System
- **OS Platform & Version**: Microsoft Windows 11 Pro Insider Preview (Build `10.0.26340`)
- **Shell Environment**: Git Bash (`MINGW64_NT-10.0-26340 3.6.10-5a1665c8.x86_64`)

## 3. Compiler & Build Flags
- **C Compiler**: `gcc.exe (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r3) 16.2.0`
- **Compiler Optimization Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`

## System Command Output
```
MINGW64_NT-10.0-26340 HelloWorld14 3.6.10-5a1665c8.x86_64 2026-09-28 10:40 UTC x86_64 Msys
```

```
lscpu unavailable (CPU logical cores: 14)
```

- **Compiler**: gcc.exe (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders, r3) 16.2.0
- **Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`
