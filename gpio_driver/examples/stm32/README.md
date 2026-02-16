# STM32 Example

This directory contains the build configuration to run the GPIO Universal Application on an **STM32F103C8T6 (Blue Pill)**.

## Overview

This example compiles the `../common/app_main.c` universal application logic against the **STM32 GPIO Implementation** (`implementations/stm32/gpioPlatform_stm32.c`) using `libopencm3`.

It also includes the **Digital Twin Bridge**:
*   GPIO changes are sent as JSON commands over **UART1** (PA9 TX, PA10 RX) at 115200 baud.
*   This allows a "Digital Twin" (Unity or Python script) to visualize the embedded device's state in real-time.

## Prerequisites

*   **Toolchain**: `arm-none-eabi-gcc`
*   **Library**: `libopencm3` (Must be installed/compiled and accessible; Check `Makefile` includes).
*   **Flasher**: `st-flash` (stlink-tools) or `openocd`.

## Build Instructions

1.  Navigate to this directory:
    ```bash
    cd examples/stm32
    ```

2.  Run Make:
    ```bash
    make
    ```

    *If `libopencm3` headers are not found, edit `Makefile` variables `INCLUDES` and `LDFLAGS`.*

## Flashing (Example)

Using ST-Link:

```bash
st-flash write build/index.bin 0x8000000
```

## Digital Twin Setup

1.  Connect a **USB-to-Serial adapter** to the STM32:
    *   Adapter RX -> STM32 PA9 (TX)
    *   Adapter TX -> STM32 PA10 (RX)
    *   GND -> GND
2.  Open the Serial Bridge script (in root `digital_twin/serial_bridge.py` - *if available*) or view raw output in a terminal (PuTTY/RealTerm) at **115200 baud**.
3.  You will see JSON lines like: `{"t":"GPIO","p":"LED1","v":1}` whenever the LED toggles.
