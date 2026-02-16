#!/bin/bash
set -e

echo "[AVR] Setting up CMake build..."

# 1. Create Build Directory
mkdir -p build_cmake

# 2. Configure
cd build_cmake
echo "[AVR] Generating Unix Makefiles..."
cmake -G "Unix Makefiles" ..

# 3. Build
echo "[AVR] Compiling..."
cmake --build .

echo "[AVR] Build Success!"
echo "[AVR] Hex file: build_cmake/gpio_example_avr.hex"
echo "[AVR] Run 'cmake --build . --target flash' inside build_cmake to flash."
