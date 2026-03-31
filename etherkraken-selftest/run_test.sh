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
if [[ -z "$DEVICE_IP" ]]; then
    echo "Invalid device IP"
    exit 1
fi

read -p "Enter device user (default 'etherkraken'): " DEVICE_USER
if [[ -z "$DEVICE_USER" ]]; then
    DEVICE_USER="etherkraken"
fi

read -sp "Enter device password (default 'etherkraken'): " DEVICE_PASSWORD
if [[ -z "$DEVICE_PASSWORD" ]]; then
    DEVICE_PASSWORD="etherkraken"
fi
echo

read -p "Enter device profiling state (default 'false'): " DEVICE_PROFILING_STATE
if [[ -z "$DEVICE_PROFILING_STATE" ]]; then
	DEVICE_PROFILING_STATE="false"
fi

if [[ "$DEVICE_PROFILING_STATE" = "false" ]]; then
	read -p "Enter device debugging state (default 'false'): " DEVICE_DEBUG_STATE
	if [[ -z "$DEVICE_DEBUG_STATE" ]]; then
    	DEVICE_DEBUG_STATE="false"
	fi
else
	DEVICE_DEBUG_STATE="false"
fi

echo

echo "Building selftest binary.."
./../gradlew clean \
    :libkraken-interop:linuxArm64Cinterop-libkrakenKlib \
    :etherkraken-selftest:linkDebugExecutableLinuxArm64 \
    --no-daemon --rerun-tasks --quiet --console=plain

echo "Copying binary to target device.."
sshpass -p $DEVICE_PASSWORD rsync -avz \
    build/bin/linuxArm64/debugExecutable/etherkraken-selftest.kexe \
    $DEVICE_USER@$DEVICE_IP:/home/$DEVICE_USER/selftest

echo "Making binary executable.."
sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP chmod +x /home/$DEVICE_USER/selftest

echo "Running selftest.."
if [[ "$DEVICE_DEBUG_STATE" = "true" ]]; then
    sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "cd /home/$DEVICE_USER && gdbserver :6767 ./selftest"
elif [[ "$DEVICE_PROFILING_STATE" = "true" ]]; then
	sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "cd /home/$DEVICE_USER && valgrind --leak-check=full ./selftest"
else
    sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "cd /home/$DEVICE_USER && ./selftest"
fi
