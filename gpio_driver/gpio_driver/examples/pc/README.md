# PC / Simulator Example

This directory contains the build configuration to run the GPIO Universal Application on a PC.

## Overview

This example compiles the `../common/app_main.c` universal application logic against the **HTTP GPIO Implementation** (`implementations/pc/gpioLib_http.c`).

This allows the C application to control a graphical component in the Python/Unity simulator over localhost HTTP.

## Prerequisites

*   **GCC**: MinGW (Windows) or standard GCC (Linux/Mac).
*   **Make**: GNU Make (`mingw32-make` on Windows).
*   **Python 3**: To run the GPIO Simulator.

## Quick Start (Digital Twin Mode)

1.  **Start the Simulator Server**:
    Open a new terminal and run:
    ```bash
    cd ../../simulator
    python gpio_simulator.py
    ```
    *Keep this running.*

2.  **Build the Example**:
    In this directory (`examples/pc`), run:
    ```bash
    make
    ```

3.  **Run the Application**:
    ```bash
    ./build/example_http.exe
    ```

You should see logs in the terminal, and if you have the Simulator GUI open (Unity or Web), the LED should toggle when you press the button (or simulate it).

## Alternative: Windows Native

To build using the native Windows API (less feedback, but no simulator required):

```bash
make PLATFORM=WINDOWS
./build/example_windows.exe
```
