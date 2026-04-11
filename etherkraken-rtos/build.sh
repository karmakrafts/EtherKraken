#!/bin/bash

set -euo pipefail

# ==================== Build configuration begin ====================

TIMESTAMP=$(date +"%Y%m%d-%H%M")
ARCH="arm64"
TARGET="aarch64-linux-gnu-"
USERNAME="etherkraken"
DEFAULT_GROUPS="users,adm,dialout,audio,netdev,video,plugdev,cdrom,games,input,gpio,spi,i2c,render,sudo"
PASSWORD="etherkraken"
HOSTNAME="etherkraken"
HASHED_PASSWORD=$(openssl passwd -6 "$PASSWORD")

NETWORK_CONFIG_FILE="network-config"
USER_DATA_FILE="boot/user-data"
META_DATA_FILE="boot/meta-data"
BOOT_CONFIG_FILE="boot/config.txt"
CMDLINE_CONFIG_FILE="boot/cmdline.txt"
REGION_CODE="DE"
REGION_CODE_LC="de"
KEYBOARD_LAYOUT="pc105"
TIMEZONE="Europe/Berlin"
BUILD_DIR="build"
MOUNT_DIR="mount"
OUTPUT_IMG_NAME="etherkraken_$TIMESTAMP.img"
ROOT_PART_NUM="2"
ROOT_PART_EXT_SIZE="128" # in MB

IMG_VERSION="2025-12-04"
IMG_KERNEL_VERSION="6.12.47"
IMG_NAME="$IMG_VERSION-raspios-trixie-$ARCH-lite.img"
IMG_URL="https://downloads.raspberrypi.com/raspios_lite_$ARCH/images/raspios_lite_$ARCH-$IMG_VERSION/$IMG_NAME.xz"

KERNEL_DEFCONFIG="bcm2711_rt_defconfig"
KERNEL_VERSION_MM="6.12"
KERNEL_VERSION="stable_20250916"
KERNEL_FILE="$KERNEL_VERSION.tar.gz"
KERNEL_DIR="linux-$KERNEL_VERSION"
KERNEL_URL="https://github.com/raspberrypi/linux/archive/refs/tags/$KERNEL_FILE"

NUM_THREADS=$(grep -c ^processor /proc/cpuinfo)

# ==================== Build configuration end ====================

echo
echo "┏━━━━━━━━━━━━━━━━━━━ EtherKraken RTOS Image Buildscript v1 ━━━━━━━━━━━━━━━━━━━┓"
printf "%-80s┃\n" "┃ Base image:    Raspberry Pi OS Lite (64 Bit) $IMG_VERSION"
printf "%-80s┃\n" "┃ Kernel:        $KERNEL_VERSION ($KERNEL_VERSION_MM)"
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

echo "Extending image root partition.."
dd if=/dev/zero bs=1M count=$ROOT_PART_EXT_SIZE >> "$OUTPUT_IMG_NAME" # Add 512MB extra space to the image
parted "$OUTPUT_IMG_NAME" resizepart $ROOT_PART_NUM 100% # Allocate all available new space to the root partition

# Mount image partitions
echo "Creating image loopback device.."
IMG_LOOP=$(losetup --show -fP "$OUTPUT_IMG_NAME")
IMG_BOOT_PART="${IMG_LOOP}p1"
IMG_ROOT_PART="${IMG_LOOP}p2"
echo "Mounting image partitions.."
mount "$IMG_ROOT_PART" "$MOUNT_DIR/root"
mount "$IMG_BOOT_PART" "$MOUNT_DIR/boot"
echo "Image partitions are $IMG_BOOT_PART and $IMG_ROOT_PART"

# Download and build Linux kernel
cd $BUILD_DIR
if [[ ! -f "$KERNEL_FILE" ]]; then
	echo "Downloading Linux kernel.."
	curl -SOL $KERNEL_URL
fi
if [[ ! -d "$KERNEL_DIR" ]]; then
	echo "Decompressing Linux kernel.."
	tar -xzf $KERNEL_FILE
fi
cd $KERNEL_DIR

echo "Building kernel.."
make ARCH=$ARCH CROSS_COMPILE=$TARGET $KERNEL_DEFCONFIG
scripts/config --enable PREEMPT_RT_FULL # Enable PREEMPT_RT
scripts/config --enable CONFIG_HZ_1000  # Enable 1000Hz high-res timers
make ARCH=$ARCH CROSS_COMPILE=$TARGET -j $NUM_THREADS Image modules dtbs

echo "Installing kernel modules.."
make ARCH=$ARCH CROSS_COMPILE=$TARGET INSTALL_MOD_PATH="../../$MOUNT_DIR/root" -j $NUM_THREADS modules_install

echo "Removing old kernel modules.."
rm -rf "../../$MOUNT_DIR/root/lib/modules/$IMG_KERNEL_VERSION+rpt-rpi-2712" # 32-bit modules
rm -rf "../../$MOUNT_DIR/root/lib/modules/$IMG_KERNEL_VERSION+rpt-rpi-v8"   # Non-PREEMPT_RT 64-bit modules

cd .. # $KERNEL_DIR
cd .. # $BUILD_DIR

echo "Copying kernel image.."
cp "$BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/Image" "$MOUNT_DIR/boot/kernel8.img"
echo "Copying device tree overlays.."
cp -r $BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/dts/broadcom/*.dtb "$MOUNT_DIR/boot"
cp -r $BUILD_DIR/$KERNEL_DIR/arch/$ARCH/boot/dts/overlays/*.dtb "$MOUNT_DIR/boot/overlays"

# Configure image defaults
echo "Writing default boot config.."
cp "$BOOT_CONFIG_FILE" "$MOUNT_DIR/boot/"

echo "Writing default cmdline config.."
tr -d '\n' < "$MOUNT_DIR/boot/cmdline.txt" > temp_cmdline.txt
mv "temp_cmdline.txt" "$MOUNT_DIR/boot/cmdline.txt"
cat "$CMDLINE_CONFIG_FILE" | tr -d '\n' >> "$MOUNT_DIR/boot/cmdline.txt"

echo "Configuring hostname.." 
echo "$HOSTNAME" > "$MOUNT_DIR/root/etc/hostname"

echo "Writing default userconf file.."
echo "$USERNAME:$HASHED_PASSWORD" > "$MOUNT_DIR/boot/userconf.txt"

echo "Writing default meta-data file.."
cp "$META_DATA_FILE" "$MOUNT_DIR/boot/"
echo -n "$TIMESTAMP" >> "$MOUNT_DIR/boot/meta-data" # Append unique build timestamp to metadata

if [[ -f "$NETWORK_CONFIG_FILE" ]]; then
	echo "Copying default network configuration.."
	cp "$NETWORK_CONFIG_FILE" "$MOUNT_DIR/boot/$NETWORK_CONFIG_FILE"
fi

echo "Writing default user-data file.."
cat > "$MOUNT_DIR/boot/user-data" << EOF
manage_resolv_conf: false
hostname: $HOSTNAME
manage_etc_hosts: true
packages:
- avahi-daemon
apt:
  preserve_sources_list: true
  conf: |
    Acquire {
      Check-Date "false";
    };
timezone: $TIMEZONE
keyboard:
  model: $KEYBOARD_LAYOUT
  layout: "$REGION_CODE_LC"
users:
- name: $USERNAME
  groups: $DEFAULT_GROUPS
  shell: /bin/bash
  lock_passwd: false
  passwd: "$HASHED_PASSWORD"
enable_ssh: true
ssh_pwauth: true
rpi:
  interfaces:
    serial: true
EOF

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