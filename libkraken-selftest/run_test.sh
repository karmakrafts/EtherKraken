#!/bin/bash

#
# Copyright 2026 Karma Krafts
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set -euo pipefail

read -p "Enter device IP: " DEVICE_IP

read -p "Enter device user (default 'etherkraken'): " DEVICE_USER
if [[ -z "$DEVICE_USER" ]]; then
    DEVICE_USER="etherkraken"
fi

read -sp "Enter device password (default 'etherkraken'): " DEVICE_PASSWORD
if [[ -z "$DEVICE_PASSWORD" ]]; then
    DEVICE_PASSWORD="etherkraken"
fi
echo
echo

echo "Building selftest binary.."
./../gradlew clean \
    :libkraken-interop:linuxArm64Cinterop-libkrakenKlib \
    :libkraken-selftest:linkDebugExecutableLinuxArm64 \
    --no-daemon --rerun-tasks --quiet --console=plain

echo "Copying binary to target device.."
sshpass -p $DEVICE_PASSWORD rsync -avz \
    build/bin/linuxArm64/debugExecutable/libkraken-selftest.kexe \
    $DEVICE_USER@$DEVICE_IP:/home/$DEVICE_USER/selftest

echo "Making binary executable.."
sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP chmod +x /home/$DEVICE_USER/selftest

echo "Running selftest.."
sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "cd /home/$DEVICE_USER && ./selftest"