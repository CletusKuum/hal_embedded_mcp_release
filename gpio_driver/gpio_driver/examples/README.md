# GPIO Driver Examples

This directory contains examples for using the GPIO Driver on various platforms.

## Structure

*   **`common/`**: Contains the universal application logic (`app_main.c`). This code is platform-agnostic and relies on a "Platform Adapter" contract.
*   **`pc/`**: Build system for PC (Windows/Linux) using the HTTP Simulator or Windows API.
    *   Uses `platform_adapter.c` to bind the application to `gpioLib_http` or `gpioLib_windows`.
*   **`stm32/`**: Build system and adapter for STM32 microcontrollers.
    *   Uses `platform_adapter.c` to bind the application to `gpioPlatform_stm32`.

## Building

### PC (Simulator/Windows)

Navigate to `examples/pc` and run:

```bash
# Default (HTTP Simulator)
make

# Windows File-Based
make PLATFORM=WINDOWS
```

### STM32

Navigate to `examples/stm32` and run:

```bash
make
```

(Note: You will need the ARM GCC toolchain installed).
