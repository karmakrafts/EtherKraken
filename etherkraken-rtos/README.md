# EtherKraken RTOS

EtherKraken RTOS is a modified version of Raspberry Pi OS Lite, incorporating  
PREEMPT_RT kernel patches, default configurations and custom software.

### Building

For building EtherKraken RTOS, a machine or VM with an Ubuntu based distribution  
is recommended. Ensure you have all dependencies installed before proceeding by
running the following command:

```shell
sudo apt install -y build-essential curl wget unzip xz-utils \
    git bc bison flex libncurses5-dev libssl-dev \
    crossbuild-essential-arm64 kmod rsync \
    device-tree-compiler parted
```

When you have all dependencies installed, simply invoke the image buildscript:

```shell
sudo ./build.sh
```

> **Note:** root permission is required to run the buildscript, since it 
> interacts with loopback devices on the build host.

The build was last successfully tested on **PopOS 24.04**.

### Configuration

The default build configuration is specified in `build.sh` directly and may  
be changed as required.  
The default user is `etherkraken` with the default password of `etherkraken`.

#### Boot

The boot configuration may be changed by adjusting `boot/config.txt` and  
`boot/cmdline.txt` accordingly.  
The latter will be appended to the default `cmdline.txt` already included  
in the base image.

#### Network

If a default network configuration should be baked into the built image,  
a `network-config` file needs to be placed in `etherkraken-rtos` root directory  
before invoking `build.sh`.

An example of this can be found in `example/network-config`.