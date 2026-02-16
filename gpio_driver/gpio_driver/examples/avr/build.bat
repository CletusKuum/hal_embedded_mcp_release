@echo off
setlocal

echo [AVR] Setting up CMake build...

:: 1. Create Build Directory
if not exist build_cmake (
    mkdir build_cmake
)

:: 2. Configure (Generate Makefiles)
cd build_cmake
echo [AVR] Generating MinGW Makefiles...
cmake -G "MinGW Makefiles" ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

:: 3. Build
echo [AVR] Compiling...
cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

echo [AVR] Build Success! 
echo [AVR] Hex file: build_cmake\gpio_example_avr.hex
echo [AVR] Run 'cmake --build . --target flash' inside build_cmake to flash.
endlocal
