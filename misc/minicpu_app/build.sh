#!/bin/bash

CWD=$(cd "$(dirname $0)"; pwd)
TOOLCHAIN_HOME="$CWD/../riscv-gnu-toolchain"
TOOLCHAIN_INSTALL_HOME="$TOOLCHAIN_HOME/install"
PATH="$TOOLCHAIN_INSTALL_HOME/bin/:$PATH"
CROSS_COMPILE="riscv64-unknown-linux-gnu-"
CC="${CROSS_COMPILE}gcc"
LD="${CROSS_COMPILE}ld"
OBJCOPY="${CROSS_COMPILE}objcopy"

BEAR="bear -- "

TEXT_ADDR="0x180000000" # 16MB
#DATA_ADDR="0x181000000" # 16MB
#BSS_ADDR="0x182000000" # 16MB

$CC -v

(
    cd "$CWD"
    $CC      -g -c ./start.S -o ./start.o -march=rv64i -mabi=lp64 -O0
    $CC      -g -c ./main.c -o ./main.o -march=rv64i -mabi=lp64 -mcmodel=medany -O0
#    $LD      -Ttext=${TEXT_ADDR} -nostdlib ./start.o ./main.o -o ./start.elf
    $LD      -T ./linker.ld  -nostdlib ./start.o ./main.o -o ./start.elf
    $OBJCOPY -O binary ./start.elf ./start.bin
)
