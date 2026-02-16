# GPIO Driver - Developer Guide (Manual)

Welcome! This guide explains how to develop with, build, and extend the GPIO driver ecosystem.

## 1. Quick Start: Standard Workflow

To add a new feature or pin, follow these steps:

1.  **Define the Pin**: Open `gpio_driver/examples/pc/config.json` and add your pin:
    ```json
    { "name": "VALVE_1", "pin": 6, "dir": "OUTPUT" }
    ```
2.  **Generate Config**: Run the generator script:
    ```bash
    python gpio_driver/scripts/gen_config.py
    ```
3.  **Use in Code**: In `app_main.c`, use the logical name:
    ```c
    eGpioHelperWrite("VALVE_1", true);
    ```
4.  **Build & Flash**:
    ```bash
    cd gpio_driver/examples/avr
    .\build.bat
    .\flash.bat COM3 115200
    ```

---

## 2. Using the GPIO Helper

Always use the **GPIO Helper** layer (`eGpioHelperWrite`/`eGpioHelperRead`) instead of direct HAL calls. The helper provides:
-   **Hardware Execution**: Toggles the physical pin.
-   **Twin Synchronization**: Automatically sends JSON over UART so Unity updates in real-time.

---

## 3. Running the Digital Twin Simulation

If you want to see your code run with a virtual model:

1.  **Start Simulator**:
    ```bash
    cd gpio_driver/simulator
    python gpio_simulator.py
    ```
2.  **Start Serial Bridge**:
    *(Ensure PuTTY is closed!)*
    ```bash
    cd helper_utils
    python serial_bridge.py
    ```
3.  **Launch Unity**: Open the `GPIO_Demo` scene and press Play.

---

## 4. Troubleshooting

### "Could not open COM port"
-   **Cause**: Another program (PuTTY, Arduino IDE) is using the port.
-   **Fix**: Close all terminal emulators and restart the `serial_bridge.py`.

### "ModuleNotFoundError: No module named 'serial'"
-   **Fix**: `pip install pyserial`

### LED is not blinking
-   **Check**: Verify `config.json` pin index matches your physical board (e.g., Pin 5 = PB5).
-   **Check**: Ensure `vHalRegisterGpioInterface` is called at the start of `main()`.

---

## 5. Adding a New Platform

To add a new MCU (e.g., ESP32):
1.  Add `gpioPlatform_esp32.c` in `implementations/`.
2.  Implement the `sGpioInterface_t` callbacks.
3.  Create a `platform_adapter.c` that initializes the ESP32 UART and registers the interface.
4.  Update your CMake to include the new source folder.
