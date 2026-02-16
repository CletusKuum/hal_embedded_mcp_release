# HAL Embedded MCP – AVR build and flash

Build and flash the MCP firmware for AVR (e.g. Arduino Uno / Nano). Pins and tools come from **`hal_embedded_mcp/config/config.json`**.

## Prerequisites

- **AVR toolchain on PATH:** `avr-gcc`, `avr-objcopy`, and (for flashing) `avrdude`.  
  **You must add these to your system PATH** before building.

### Add AVR tools to PATH

- **Windows + Arduino IDE:**  
  Add the **bin** folder that contains `avr-gcc.exe` and `avrdude.exe` to your user or system PATH.  
  - Standalone installer: e.g. `C:\Program Files\Arduino\hardware\tools\avr\bin` or `%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin` (and the matching avrdude folder).  
  - Microsoft Store IDE: e.g. `C:\Program Files\WindowsApps\ArduinoLLC.ArduinoIDE_1.8.57.0_x86__mdqgnx93n4wtt\hardware\tools\avr\bin`.  
  If avrdude cannot find its config file, set **AVRDUDE_CONF** to the path to `avrdude.conf` (e.g. `...\hardware\tools\avr\etc\avrdude.conf`).

- **Windows + MSYS2:**  
  In the UCRT64 shell: `pacman -S mingw-w64-ucrt-x86_64-avr-gcc`.  
  Then run the build from a shell that has MSYS2’s `bin` on PATH (or add `C:\msys64\ucrt64\bin` to PATH).

- **Linux:**  
  `sudo apt install gcc-avr avr-libc avrdude` (or equivalent). The tools are then on PATH.

- **macOS:**  
  `brew install avr-gcc avrdude`. The tools are then on PATH.

**Optional (any OS):** Set **AVRDUDE_EXE** and **AVRDUDE_CONF** if you do not want to add avrdude to PATH (e.g. `set AVRDUDE_EXE=C:\...\avrdude.exe` and `set AVRDUDE_CONF=C:\...\avrdude.conf`).

- **CMake** (3.10+)
- **Python 3** (for config codegen at build time)
- **Windows:** MinGW so CMake can generate "MinGW Makefiles"

## Build

From this directory (`hal_embedded_mcp/mcu/avr/`):

**Windows (cmd):**
```bat
build.bat
```

**Linux / macOS:**
```bash
./build.sh
```

This creates `build_cmake/`, configures with CMake, and compiles. Output:

- `build_cmake/hal_mcp_avr.elf`
- `build_cmake/hal_mcp_avr.hex`

**Clean and reconfigure:**  
- **PowerShell:** `Remove-Item -Recurse -Force build_cmake`  
- **Cmd:** `rmdir /s /q build_cmake`  
Then run `build.bat` again.

## Flash

Connect the board via USB and note the serial port (e.g. `COM3` on Windows, `/dev/ttyUSB0` on Linux).

**Windows (cmd):**
```bat
flash.bat
```
Uses default port `COM3` and baud `57600`. To override:
```bat
flash.bat COM5 57600
```

**Linux / macOS:**
```bash
./flash.sh
```
Uses default port `/dev/ttyUSB0` and baud `57600`. To override:
```bash
./flash.sh /dev/ttyACM0 57600
```

## Run / test over serial

1. Flash the board (see above).
2. Open a serial terminal at **57600** baud (same as in `config.json`).
3. Send a line per command; the MCU dispatches to MCP tools and prints a response.

Examples:

| Send (one line)      | Effect                          | Example response |
|----------------------|----------------------------------|------------------|
| `gpio_write LED1 1`  | Turn LED1 on                     | `OK`             |
| `gpio_write LED1 0`  | Turn LED1 off                    | `OK`             |
| `gpio_read BUTTON1` | Read BUTTON1                     | `GPIO_READ BUTTON1 0` or `1` |

Pin names (e.g. `LED1`, `BUTTON1`) must match **`config/config.json`**. Digital Twin JSON lines (starting with `{`) are handled by the same firmware for simulator sync.

## Directory layout

- **build.bat** / **build.sh** – configure and compile.
- **flash.bat** / **flash.sh** – flash the built `.hex` to the board.
- **CMakeLists.txt** – build definition; uses `config/config.json` for GPIO and MCP codegen.
- **build_cmake/** – created by build; contains generated sources and `hal_mcp_avr.hex`.
