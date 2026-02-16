#!/bin/bash
set -e

echo "[HAL MCP AVR] Setting up CMake build..."

# 1. Create Build Directory
mkdir -p build_cmake

# 2. Configure
cd build_cmake
echo "[HAL MCP AVR] Generating Unix Makefiles..."
cmake -G "Unix Makefiles" ..

# 3. Build
echo "[HAL MCP AVR] Compiling..."
cmake --build .

echo "[HAL MCP AVR] Build Success!"
echo "[HAL MCP AVR] Hex file: build_cmake/hal_mcp_avr.hex"
echo "[HAL MCP AVR] Run ./flash.sh [PORT] to flash."
cd ..
