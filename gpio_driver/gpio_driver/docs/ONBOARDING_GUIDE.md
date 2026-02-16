# HAL Drivers Onboarding Guide

Welcome to the **HAL Drivers** project! This guide will walk you through the architecture and get you running your first "Digital Twin" simulation using the GPIO driver.

## Prerequisites
*   **GCC Compiler** (MinGW for Windows or build-essential for Linux).
*   **Python 3.x** (for the simulator).
*   **Unity Hub & Unity 2022.3+** (LTS Recommended).
    *   *Tip: Ensure your Unity Editor version matches the project serialization to avoid re-import delays.*

## The "Hello World" of HAL (Running the Demo)

We will use the **GPIO Driver** to demonstrate the system. You will run three separate components that talk to each other.

### Step 1: Start the Simulator
The simulator acts as the "virtual hardware" state manager.

1.  Open a terminal.
2.  Navigate to: `hal_drivers/gpio_driver/simulator/`
3.  Run the script:
    ```bash
    python gpio_simulator.py
    ```
4.  You should see: `>>> HTTP Server is READY and listening for requests!`

### Step 2: Run the C Application
This is the actual embedded code running on your PC.

1.  Open a **second** terminal.
2.  Navigate to: `hal_drivers/gpio_driver/examples/`
3.  Compile the example (linking the logger):
    ```bash
    gcc example_main.c ../../logging_driver/logLib.c -o example.exe
    ```
    *(Note: On Windows with standard MinGW, you might need `-lws2_32` if you expand the socket code later. On Linux/Mac, use `-o example` and `./example`)*
4.  Run it:
    ```bash
    ./example.exe
    ```
5.  You should see logs indicating:
    *   Logging initialized.
    *   GPIO interface registered (HTTP mode).
    *   Monitoring button state.

### Step 3: Launch Visualization (Unity)
Now we see what's happening.

1.  Open **Unity Hub**.
2.  Click **Add** -> **Add project from disk**.
3.  Select the folder: `hal_drivers/gpio_driver/gpio_digital_twin`.
4.  Open the project (this may take a few minutes to import first time).
5.  Open the Scene: `Assets/Scenes/GpioDemo.unity` (or similar).
6.  Press **Play**.

### Step 4: Interaction
1.  In the Unity Game view, find the object labeled **BUTTON1**.
2.  Click/Hold it.
3.  **Observe**:
    *   **Unity:** The button depresses visually.
    *   **Python Terminal:** Logs `POST /api/gpio/BUTTON1`.
    *   **C App Terminal:** Logs `Button pressed -> LED ON`.
    *   **Unity:** The **LED1** object lights up (receiving the state change from the C app).

## Understanding the Code
*   **If you want to change logic:** Edit `examples/example_main.c`. This is where the "Brain" is.
*   **If you want to add pins:** Edit `simulator/gpio_simulator.py` manually OR just use them in code (the simulator auto-creates pins on first access).
*   **If you want to change looks:** Edit the Unity Prefabs.

## Contributing
1.  **Pick a Driver:** Look at folders like `adc_driver` or `pwm_driver`.
2.  **Follow the Pattern:**
    *   Create a `simulator` folder with a Python script.
    *   Create a `digital_twin` folder with a Unity project.
    *   Implement the `_http.h` bridge in the driver.
3.  **Test:** verify the loop as shown above.
