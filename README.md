# EtherKraken

A RP3A0 SiP based controller board with 38 programmable IOs, 2MB of flash, SPI, I²C, UART and 100Base-T connectivity
for embedded- and automation-applications.

<img src="docs/board_render.png" alt="3D render of the circuit board" width="75%">

### Building libkraken

In order to build the `libkraken` hardware abstraction layer library,  
a computer or VM with a Debian/Ubuntu based Linux distribution is recommended.

The following dependencies are required:

```
build-essential clang-20 llvm-20 make cmake
```

You will also need the Clang RT libraries from an LLVM aarch64 sysroot,  
which you can obtain by downloading a prebuilt LLVM package [from here](https://github.com/llvm/llvm-project/releases)
and  
copying the `aarch-64-unknown-linux-gnu` directory in `LLVM-XX.X.X-Linux-ARM64/lib/clang/XX/lib`  
to `/usr/lib/llvm-XX/lib/clang/XX/lib`.

When you have all dependencies installed, simply invoke `build.sh` in the `libkraken` directory.

The toolchain configuration can be found in `libkraken/cmake/toolchain.cmake` and may be customized as desired.

### Building etherkraken-core

`etherkraken-core` is the high level Kotlin/Native wrapper around the `libkraken` HAL.  
In order to build it, you only need [a JDK](https://www.azul.com/downloads/?package=jdk#zulu) on an Ubuntu/Debian based
Linux distribution.

Simply run the following command in the root of the repository:

```shell
./gradlew build
```