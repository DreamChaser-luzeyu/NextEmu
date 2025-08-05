#!/bin/bash

CWD=$(cd "$(dirname $0)"; pwd)
TOOLCHAIN_HOME="$CWD/../misc/riscv-gnu-toolchain"
TOOLCHAIN_INSTALL_HOME="$TOOLCHAIN_HOME/install"

PATH="$TOOLCHAIN_INSTALL_HOME/bin/:$PATH"

BEAR="bear -- "
CLEAN_LINUX="true"
BUSYBOX_MENUCONFIG="false"
LINUX_MENUCONFIG="false"

echo "[Debug] Using toolchain: $(which riscv64-unknown-linux-gnu-gcc)"

# --- Build BusyBox
(
    cd "$CWD/../misc/busybox/"
#     ARCH=riscv64 CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig
    test "$BUSYBOX_MENUCONFIG" == "true" && ARCH=riscv64 CROSS_COMPILE=riscv64-unknown-linux-gnu- make menuconfig
    CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv64 make -j$(nproc) && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- ARCH=riscv64 make install
)

# --- Build Rootfs
(
    mkdir -p "$CWD/../misc/rootfs/"
    cd "$CWD/../misc/rootfs/"
    cp -r "$CWD"/../misc/busybox/_install/* .
    mkdir dev
    (
        cd dev
        sudo mknod -m 666 console c 5 1
        sudo mknod -m 666 null c 1 3
    )
    touch test_file_new

    mkdir -p ./etc/init.d/
    cat << EOF > ./etc/init.d/rcS
#!/bin/sh

echo "Init running..."
export PATH="/bin:/sbin"

# Check and create directory
[ -d /dev  ] || mkdir -m 0755 /dev
[ -d /root ] || mkdir --mode=0700 /root
[ -d /sys  ] || mkdir /sys
[ -d /proc ] || mkdir /proc
[ -d /tmp  ] || mkdir /tmp
[ -d /mnt  ] || mkdir /mnt

# Mount /proc and /sys:
mount -n proc /proc -t proc
mount -n sysfs /sys -t sysfs

# Create basic character device nodes
[ -e /dev/console ] || mknod /dev/console c 5 1
[ -e /dev/null ] || mknod /dev/null c 1 3

# Specify path to hotplug script
# echo /sbin/mdev > /proc/sys/kernel/hotplug

# Run mdev, do device scanning and create dev nodes accordingly
mdev -s

EOF
    cp -r "$CWD"/../misc/rootfs_addons/* "$CWD"/../misc/rootfs/
    chmod +x ./etc/init.d/rcS
    chmod +x ./sbin/*
    chmod +x ./bin/*
)

# --- Build Linux kernel
(
    cd "$CWD/../misc/linux-6.6"
    echo "[Debug] Building linux : $(pwd)"
    test "$CLEAN_LINUX" == "true" && ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make clean
    ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig
    sed -i 's|CONFIG_CMDLINE=""|CONFIG_CMDLINE="earlycon=uartlite,mmio,0x60100000 console=ttyUL0 rdinit=/sbin/init"|g' ./.config
    sed -i "s~CONFIG_INITRAMFS_SOURCE=\"\"~CONFIG_INITRAMFS_SOURCE=\"$CWD/../misc/rootfs/\"~g" ./.config
    echo "CONFIG_SERIAL_UARTLITE=y" >> ./.config
    echo "CONFIG_SERIAL_UARTLITE_CONSOLE=y" >> ./.config
    echo "CONFIG_SERIAL_UARTLITE_NR_UARTS=1" >> ./.config
    sed -i 's|CONFIG_EFI=y|CONFIG_EFI=n|g' ./.config
#     sed -i 's|CONFIG_RISCV_ISA_C=y|CONFIG_RISCV_ISA_C=n|g' ./.config
    test "$LINUX_MENUCONFIG" == "true" && ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make menuconfig
    ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- $BEAR make -j$(nproc)
    test "$CLEAN_LINUX" == "true" && cp ./compile_commands.json ./compile_commands.json.bak
)

# --- Build DTB
(
    cd "$CWD/../misc/"
    dtc ./simple.dts -o ./platform.dtb
)

# --- Build OpenSBI
(
    cd "$CWD/../misc/opensbi"
    echo "[Debug] Building OpenSBI : $(pwd)"
    ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make clean
    ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- make \
        PLATFORM=generic \
        PLATFORM_RISCV_ISA=rv64ima_zicsr_zifencei \
        FW_FDT_PATH="$CWD/../misc/platform.dtb" \
        FW_PAYLOAD_PATH="$CWD/../misc/linux-6.6/arch/riscv/boot/Image"
)
