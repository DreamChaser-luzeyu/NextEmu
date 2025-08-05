# NextEmu

NextEmu是一个模块化的、易懂的、基于解释执行的RISC-V全系统模拟器，受到cyyself/cemu的启发。

下面列出几个常用目录：

- `example/`：全系统的配置示例
- `misc/`：用到的第三方子模块

## 入门

### 克隆仓库

```bash
git clone https://github.com/DreamChaser-luzeyu/NextEmu.git
cd NextEmu
git submodule update --init --recursive
```

### 构建RISC V工具链

```bash
cd NextEmu
./scripts/build_toolchain.sh
```

### 构建MiniCPU程序

```bash
cd NextEmu
./misc/minicpu_app/build.sh
```

## 在MiniCPU上运行程序

MiniCPU上的程序源码位于`misc/minicpu_app`中，目前包含`start.S`和`main.c`，`misc/minicpu_app/build.sh`描述了其构建流程。

调试方法见`src/gdb_stub/README.md`

## 所使用到的开源项目

- riscv-isa-sim (spike ISS)
- riscv-gnu-toolchain
- RVVM

- OpenSBI
- Linux
- BusyBox