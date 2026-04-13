#!/bin/bash

LINKER_FILE="linker.ld"

set -euo pipefail

# Read in device definitions from device data file
python3 $1/scripts/genlink.py $1/ld/devices.data $2 DEFS | sed 's/-D//g'