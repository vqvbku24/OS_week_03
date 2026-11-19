# Experimental Environment

This document records the hardware specifications, operating system, and compiler configuration of the Linux VM used to run the final benchmarks (raw outputs from `uname -a`, `lscpu`, and `gcc --version` are appended below by `scripts/run_bench.sh`).

## 1. Hardware Specifications & CPU Topology
- **CPU Model**: Intel(R) Core(TM) Ultra 5 225U (Oracle VirtualBox VM, KVM)
- **Physical Cores / Logical Cores (SMT)**: 6 vCPUs (1 socket, 6 cores per socket, 1 thread per core)
- **L1 / L2 / L3 Cache Hierarchy**: L1d 192 KiB (6 instances), L1i 384 KiB (6 instances), L2 12 MiB (6 instances), L3 72 MiB (6 instances)
- **System Memory (RAM)**: *(Check with `free -h` on VM if exact value is needed)*

## 2. Operating System
- **OS Platform & Version**: Ubuntu 24.04.1 LTS (x86_64, Oracle VirtualBox)
- **Kernel Version**: `7.0.0-43-generic` (`#43~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC`)

## 3. Compiler & Build Flags
- **C Compiler**: `gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0`
- **Compiler Optimization Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`

## System Command Output
```
Linux harry-VirtualBox 7.0.0-43-generic #43~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Sat Oct 31 06:52:47 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
```

```
Architecture:                            x86_64
CPU op-mode(s):                          32-bit, 64-bit
Address sizes:                           46 bits physical, 48 bits virtual
Byte Order:                              Little Endian
CPU(s):                                  6
On-line CPU(s) list:                     0-5
Vendor ID:                               GenuineIntel
Model name:                              Intel(R) Core(TM) Ultra 5 225U
CPU family:                              6
Model:                                   181
Thread(s) per core:                      1
Core(s) per socket:                      6
Socket(s):                               1
Stepping:                                0
BogoMIPS:                                6144.00
Flags:                                   fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 ht syscall nx rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc cpuid tsc_known_freq pni pclmulqdq ssse3 fma cx16 pcid sse4_1 sse4_2 movbe popcnt aes xsave avx f16c rdrand hypervisor lahf_lm abm 3dnowprefetch ibrs_enhanced fsgsbase bmi1 avx2 bmi2 invpcid rdseed adx clflushopt sha_ni arat md_clear flush_l1d arch_capabilities
Hypervisor vendor:                       KVM
Virtualization type:                     full
L1d cache:                               192 KiB (6 instances)
L1i cache:                               384 KiB (6 instances)
L2 cache:                                12 MiB (6 instances)
L3 cache:                                72 MiB (6 instances)
NUMA node(s):                            1
NUMA node0 CPU(s):                       0-5
Vulnerability Gather data sampling:      Not affected
Vulnerability Ghostwrite:                Not affected
Vulnerability Indirect target selection: Mitigation; Aligned branch/return thunks
Vulnerability Itlb multihit:             Not affected
Vulnerability L1tf:                      Not affected
Vulnerability Mds:                       Not affected
Vulnerability Meltdown:                  Not affected
Vulnerability Mmio stale data:           Not affected
Vulnerability Old microcode:             Not affected
Vulnerability Reg file data sampling:    Not affected
Vulnerability Retbleed:                  Mitigation; Enhanced IBRS
Vulnerability Spec rstack overflow:      Not affected
Vulnerability Spec store bypass:         Vulnerable
Vulnerability Spectre v1:                Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:                Mitigation; Enhanced / Automatic IBRS; PBRSB-eIBRS SW sequence; BHI SW loop, KVM SW loop
Vulnerability Srbds:                     Not affected
Vulnerability Tsa:                       Not affected
Vulnerability Tsx async abort:           Not affected
Vulnerability Vmscape:                   Not affected
```

- **Compiler**: gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- **Flags**: `-O2 -std=c11 -Wall -Wextra -pthread`
