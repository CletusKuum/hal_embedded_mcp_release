@echo off
setlocal

echo [HAL MCP AVR] Setting up CMake build...

:: 1. Create Build Directory
if not exist build_cmake (
    mkdir build_cmake
)

:: 2. Configure (Generate Makefiles)
cd build_cmake
echo [HAL MCP AVR] Generating MinGW Makefiles...
cmake -G "MinGW Makefiles" ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

:: 3. Build
echo [HAL MCP AVR] Compiling...
cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

echo [HAL MCP AVR] Build Success!
echo [HAL MCP AVR] Hex file: build_cmake\hal_mcp_avr.hex
echo [HAL MCP AVR] Run flash.bat [PORT] [BAUD] to flash, or: cmake --build . --target flash
endlocal
