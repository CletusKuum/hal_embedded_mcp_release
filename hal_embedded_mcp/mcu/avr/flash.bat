@echo off
SET PORT=%1
IF "%PORT%"=="" SET PORT=COM3

SET BAUD=%2
IF "%BAUD%"=="" SET BAUD=57600

echo [FLASH] Configuring port to %PORT% at %BAUD% baud...
cd build_cmake || exit /b 1

REM Update CMake cache with new port/baud if needed (fast)
cmake -DPORT=%PORT% -DBAUD=%BAUD% .. >nul 2>&1

echo [FLASH] Flashing HAL MCP AVR to %PORT%...
cmake --build . --target flash

IF %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Flash failed! Check connections and COM port.
    cd ..
    exit /b 1
)

echo [SUCCESS] Flash complete.
cd ..
