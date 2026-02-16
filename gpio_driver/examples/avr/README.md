# AVR Example (Pure C)

This directory contains the build configuration to run the GPIO Universal Application on **ATmega328P** (e.g., Arduino Uno) using standard C and `avr-gcc`.

## Overview

*   **Implementation**: `implementations/avr/gpioPlatform_avr.c` (Direct Register Access)
*   **Adapter**: `platform_adapter.c` (UART Bridge for Digital Twin)
*   **Build System**: Standard GNU Make

## Prerequisites

*   `avr-gcc`
*   `avr-libc`
*   `avrdude` (for flashing)
*   `make`

## Build Instructions

1.  Navigate to this directory:
    ```bash
    cd examples/avr
    ```

2.  Run Make:
    ```bash
    make
    ```

3.  Flash (Example for Arduino Uno on COM3):
    ```bash
    # Default COM3
    make flash
    
    # Custom Port
    make flash PORT=COM5
    ```

## Build with CMake (Cross-Platform)

You can use standard CMake commands on any OS (Windows, Linux, macOS).

**1. Generate & Build**
```bash
# Windows (PowerShell)
mkdir build_cmake; cd build_cmake
cmake -G "MinGW Makefiles" ..
cmake --build .

# Linux / Mac / Bash
mkdir -p build_cmake && cd build_cmake
cmake -G "Unix Makefiles" ..
cmake --build .
```

**2. Flash**
```bash
cmake --build . --target flash
# To specify port: cmake -DPORT=COM5 ..
```

### Helper Scripts (Optional)
If you prefer scripts, we provide both:
- **Windows**: `build.bat`
- **Linux/Mac**: `./build.sh`

## Manual Flash

Use the provided scripts for quick testing:

**Windows:**
```cmd
flash.bat COM4
```

**Linux / Mac:**
```bash
./flash.sh /dev/ttyUSB0
```

## Digital Twin

The application sends JSON state updates over UART (8N1, 115200 baud).

```json
{"t":"GPIO","p":"LED1","v":1}
```

You can view this in any serial monitor or use the Python Digital Twin bridge.

## Troubleshooting Flashing

If you encounter `stk500_getsync()` errors or "Access is denied":

### 1. Baud Rate Mismatch
*   **Standard Nano (New Bootloader)**: uses **115200**.
*   **Nano (Old Bootloader)**: uses **57600**.
*   **CRITICAL**: Some clones blink **only once** (like Optiboot) but still require **57600**. Do not rely solely on the LED blink count. If 115200 fails, try 57600.

### 2. FTDI Driver Latency (Windows)
If using an FTDI chip (e.g., generic Nano):
1.  Open **Device Manager** -> Ports (COM & LPT) -> USB Serial Port -> Properties.
2.  **Port Settings** -> **Advanced**.
3.  Change **Latency Timer** from 16ms to **1ms**.
4.  Replug the device.

### 3. "Access is denied" / "I/O operation aborted"
This usually indicates a **hardware failure** (USB cable or Port).
*   The USB connection is dropping mid-transfer.
*   **Solution**: Swap the Mini-USB cable or try a different USB port (preferably USB 2.0).
