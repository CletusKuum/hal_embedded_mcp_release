#!/bin/bash
PORT=${1:-/dev/ttyUSB0}
BAUD=${2:-57600}

echo "[FLASH] Flashing HAL MCP AVR to $PORT at $BAUD baud..."
cd build_cmake || exit 1

cmake -DPORT="$PORT" -DBAUD="$BAUD" .. >/dev/null 2>&1
cmake --build . --target flash

if [ $? -ne 0 ]; then
    echo "[ERROR] Flash failed! Check connections and serial port."
    cd ..
    exit 1
fi

echo "[SUCCESS] Flash complete."
cd ..
