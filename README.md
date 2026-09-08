# rivoss

**RI**SC-**V** **OS** **S**imulator

## Prerequisites

- Clang, LLVM, and lld. Alternatively you can use [GNU RISC-V toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain), configured with arch `rv32i` and ABI `ilp32`. This is available as a package on some systems otherwise you will have to compile it which can take a *while*.
- GCC, >= 16.2.1, supporting C++ >= 23
- CMake
- Git

## Compiling programs

In order to compile a single program targeting the simulator, use the following command:

```
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 -nostdlib -ffreestanding -fuse-ld=lld -T link.ld -o out.elf start.S test.c
```

Or alternatively with GCC:
```
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles -ffreestanding -T link.ld main.c -o main.elf
```