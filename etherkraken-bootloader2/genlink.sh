#!/bin/bash

LINKER_FILE="linker.ld"

set -euo pipefail

# Read in device definitions from device data file
DEFS=$(python3 $1/scripts/genlink.py $1/ld/devices.data $2 DEFS)

# Invoke preprocessor on linker script template
arm-none-eabi-gcc -E -P $DEFS $1/ld/$LINKER_FILE.S -o $LINKER_FILE