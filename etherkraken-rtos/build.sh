#!/bin/bash

set -euo pipefail

# ==================== Build configuration begin ====================

ARCH="arm64"
TARGET="aarch64-linux-gnu-"
TIMESTAMP=$(date +"%Y%m%d-%H%M")
LOCAL_VERSION="etherkraken_$TIMESTAMP"
USERNAME="etherkraken"
PASSWORD="etherkraken"
HOSTNAME="etherkraken"

NETWORK_CONFIG_FILE="network-config"
BOOT_CONFIG_FILE="boot/config.txt"
CMDLINE_CONFIG_FILE="boot/cmdline.txt"
BUILD_DIR="build"
MOUNT_DIR="mount"
OUTPUT_IMG_NAME="$LOCAL_VERSION.img"

IMG_VERSION="2025-12-04" # 6.12.47
IMG_NAME="$IMG_VERSION-raspios-trixie-$ARCH-lite.img"
IMG_URL="https://downloads.raspberrypi.com/raspios_lite_$ARCH/images/raspios_lite_$ARCH-$IMG_VERSION/$IMG_NAME.xz"

KERNEL_DEFCONFIG="bcm2711_rt_defconfig"
KERNEL_VERSION_MM="6.12"
KERNEL_VERSION="stable_20250916"
KERNEL_FILE="$KERNEL_VERSION.tar.gz"
KERNEL_DIR="linux-$KERNEL_VERSION"
KERNEL_URL="https://github.com/raspberrypi/linux/archive/refs/tags/$KERNEL_FILE"

RT_PATCH_VERSION="6.12.43-rt12"
RT_PATCH_FILE="patch-$RT_PATCH_VERSION.patch.xz"
RT_PATCH_URL="https://cdn.kernel.org/pub/linux/kernel/projects/rt/$KERNEL_VERSION_MM/older/$RT_PATCH_FILE"

NUM_THREADS=$(grep -c ^processor /proc/cpuinfo)

# ==================== Build configuration end ====================

echo
echo "┏━━━━━━━━━━━━━━━━━━━ EtherKraken RTOS Image Buildscript v1 ━━━━━━━━━━━━━━━━━━━┓"
printf "%-80s┃\n" "┃ Base image:    Raspberry Pi OS Lite (64 Bit) $IMG_VERSION"
printf "%-80s┃\n" "┃ Kernel:        $KERNEL_VERSION ($KERNEL_VERSION_MM)"
printf "%-80s┃\n" "┃ RT Patch:      $RT_PATCH_VERSION"
printf "%-80s┃\n" "┃ Build time:    $TIMESTAMP"
printf "%-80s┃\n" "┃ Build threads: $NUM_THREADS"
echo "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛"
echo

# Ensure build directory exists
if [[ ! -d "$BUILD_DIR" ]]; then
	echo "Creating build directory.."
	mkdir $BUILD_DIR
fi
if [[ ! -d "$MOUNT_DIR" ]]; then
	echo "Creating mount directories.."
	mkdir $MOUNT_DIR
	mkdir "$MOUNT_DIR/boot"
	mkdir "$MOUNT_DIR/root"
fi

# Download unmodified base image and create a copy
if [[ ! -f "$IMG_NAME" ]]; then
	echo "Downloading base image.."
	curl -SOL $IMG_URL
	echo "Decompressing base image.."
	xz -d "$IMG_NAME.xz"
fi
echo "Creating a copy of the base image.."
cp $IMG_NAME $OUTPUT_IMG_NAME

# Mount image partitions
echo "Creating image loopback device.."
IMG_LOOP=$(losetup --show -fP "$OUTPUT_IMG_NAME")
IMG_BOOT_PART="${IMG_LOOP}p1"
IMG_ROOT_PART="${IMG_LOOP}p2"
echo "Mounting image partitions.."
mount "$IMG_ROOT_PART" "$MOUNT_DIR/root"
mount "$IMG_BOOT_PART" "$MOUNT_DIR/boot"
echo "Image partitions are $IMG_BOOT_PART and $IMG_ROOT_PART"

# Download, patch and build Linux kernel
cd $BUILD_DIR
if [[ ! -f "$KERNEL_FILE" ]]; then
	echo "Downloading Linux kernel.."
	curl -SOL $KERNEL_URL
fi
if [[ -d "$KERNEL_DIR" ]]; then
	echo "Deleting old kernel sources.."
	rm -rf $KERNEL_DIR
fi
echo "Decompressing Linux kernel.."
tar -xzf $KERNEL_FILE
if [[ ! -f "$RT_PATCH_FILE" ]]; then
	echo "Downloading RT patch.."
	curl -SOL $RT_PATCH_URL
fi
cd $KERNEL_DIR

echo "Applying RT patch"
xzcat ../$RT_PATCH_FILE | patch -p1 -s --forward || true

echo "Building patched kernel.."
make ARCH=$ARCH CROSS_COMPILE=$TARGET LOCALVERSION=$LOCAL_VERSION $KERNEL_DEFCONFIG
scripts/config --enable PREEMPT_RT_FULL # Enable PREEMPT_RT
scripts/config --enable CONFIG_HZ_1000  # Enable 1000Hz high-res timers
make ARCH=$ARCH CROSS_COMPILE=$TARGET LOCALVERSION=$LOCAL_VERSION -j $NUM_THREADS Image modules dtbs

echo "Installing patched kernel modules.."
make ARCH=$ARCH CROSS_COMPILE=$TARGET LOCALVERSION=$LOCAL_VERSION INSTALL_MOD_PATH="$MOUNT_DIR/root/"

cd .. # $KERNEL_DIR
cd .. # $BUILD_DIR

echo "Copying patched kernel image.."
cp "$BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/Image" "$MOUNT_DIR/boot/kernel8.img"
echo "Copying device tree overlays.."
cp -r $BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/dts/broadcom/*.dtb "$MOUNT_DIR/boot/"
cp -r $BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/dts/overlays/*.dtb "$MOUNT_DIR/boot/overlays/"

# Configure image defaults
echo "Writing default boot config.."
cp "$BOOT_CONFIG_FILE" "$MOUNT_DIR/boot/"

echo "Writing default cmdline config.."
printf " %s" "$(tr '\n' ' ' < $CMDLINE_CONFIG_FILE)" >> "$MOUNT_DIR/boot/cmdline.txt"

echo "Configuring hostname.."
echo "$HOSTNAME" > "$MOUNT_DIR/root/etc/hostname"

echo "Generating default user.."
PASSWORD_HASH=$(openssl passwd -6 "$PASSWORD")
echo "$USERNAME:$PASSWORD_HASH" > "$MOUNT_DIR/boot/userconf.txt"

echo "Enabling SSH.."
touch "$MOUNT_DIR/boot/ssh"

if [[ -f "$NETWORK_CONFIG_FILE" ]]; then
	echo "Copying default network configuration.."
	cp "$NETWORK_CONFIG_FILE" "$MOUNT_DIR/boot/$NETWORK_CONFIG_FILE"
fi

# Unmount image partitions
while lsof "$IMG_ROOT_PART" >/dev/null 2>&1; do
    echo "Device $IMG_ROOT_PART is busy, waiting..."
    sleep 2
done
while lsof "$IMG_BOOT_PART" >/dev/null 2>&1; do
    echo "Device $IMG_BOOT_PART is busy, waiting..."
    sleep 2
done

echo "Unmounting image partitions.."
umount "$IMG_ROOT_PART"
umount "$IMG_BOOT_PART"
echo "Deleting image loopback device.."
losetup -d "$IMG_LOOP"