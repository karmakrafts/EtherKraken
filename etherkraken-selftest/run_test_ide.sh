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

DEVICE_IP=""
DEVICE_USER=""
DEVICE_PASSWORD=""
DEVICE_DEBUG_STATE=""

while getopts "a:u:p:d:" opt; do
    case $opt in
        a)
            DEVICE_IP="$OPTARG"
            ;;
        u)
            DEVICE_USER="$OPTARG"
            ;;
        p)
            DEVICE_PASSWORD="$OPTARG"
            ;;
        d)
            DEVICE_DEBUG_STATE="$OPTARG"
            ;;
        *)
            echo "Ignoring unknown argument -$opt"
            ;;
    esac
done

if [[ -z "$DEVICE_IP" ]]; then
    echo "Invalid device IP"
    exit 1
fi
if [[ -z "$DEVICE_USER" ]]; then
    DEVICE_USER="etherkraken"
fi
if [[ -z "$DEVICE_PASSWORD" ]]; then
    DEVICE_PASSWORD="etherkraken"
fi
if [[ -z "$DEVICE_DEBUG_STATE" ]]; then
    DEVICE_DEBUG_STATE="false"
fi

echo "Building selftest binary.."
./../gradlew clean \
    :libkraken-interop:linuxArm64Cinterop-libkrakenKlib \
    :libkraken-selftest:linkDebugExecutableLinuxArm64 \
    --no-daemon --rerun-tasks --quiet --console=plain

echo "Copying binary to target device.."
sshpass -p $DEVICE_PASSWORD rsync -avz \
    build/bin/linuxArm64/debugExecutable/libkraken-selftest.kexe \
    $DEVICE_USER@$DEVICE_IP:/home/$DEVICE_USER/selftest

echo "Making binary executable and setting process permissions.."
sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "chmod +x /home/$DEVICE_USER/selftest && sudo setcap cap_sys_nice+ep /home/$DEVICE_USER/selftest"

echo "Running selftest.."
if [[ "$DEVICE_DEBUG_STATE" = "true" ]]; then
    # Launch gdbserver on remote device in the background and disconnect stdin so SSH can disconnect
    echo "Launching remote GDB server.."
    sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP \
        "nohup gdbserver :6767 /home/$DEVICE_USER/selftest > /tmp/gdbserver.log 2>&1 < /dev/null & disown"
else
    sshpass -p $DEVICE_PASSWORD ssh $DEVICE_USER@$DEVICE_IP "cd /home/$DEVICE_USER && ./selftest"
fi