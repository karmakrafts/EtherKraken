# EtherKraken bootloader2

bootloader2 (or BL2) is a second stage bootloader for the co-processor  
on the EtherKraken controller board,  
which allows sideloading and flashing firmware via UART to facilitate debugging  
and updates coordinated by the main processor.

### Building

For building EtherKraken bootloader2, a machine or VM with an Ubuntu based distribution  
is recommended. Ensure you have all dependencies installed before proceeding by
running the following command:

```shell
sudo apt install -y build-essential curl wget unzip xz-utils \
	git cmake python3 clang-20 llvm-20 gcc-arm-none-eabi
```

When you have all dependencies installed, simply invoke the image buildscript:

```shell
./build.sh
```