#!/bin/bash

CWD=$(cd "$(dirname $0)"; pwd)
TOOLCHAIN_HOME="$CWD/../misc/riscv-gnu-toolchain"

# echo $TOOLCHAIN_HOME

(
    cd $TOOLCHAIN_HOME
    echo "[Debug] Toolchain home: $(pwd)"

    git submodule update --init --recursive

    mkdir -p ./build
    mkdir -p ./install

    (
        cd build
        make clean
        # --- With multilib, super slow!
#         ../configure \
#             --prefix="$TOOLCHAIN_HOME/install" \
#             --with-arch=rv64ima \
#             --with-abi=lp64 \
#             --enable-multilib && \
#         make linux -j$(nproc) && \
#         make newlib -j$(nproc)
#         ../configure \
#             --prefix="$TOOLCHAIN_HOME/install" \
#             --with-arch=rv64imac \
#             --with-abi=lp64 && \
#         make linux -j$(nproc)
        ../configure \
            --prefix="$TOOLCHAIN_HOME/install" \
            --with-arch=rv64imac \
            --with-abi=lp64 && \
        make linux -j$(nproc) && \
        make -j$(nproc)
    )

)
