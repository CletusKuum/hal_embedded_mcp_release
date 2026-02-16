# GPIO Library - Simple HAL Interface for Embedded Systems

A lightweight, hardware-agnostic GPIO library for embedded systems with a simple HAL interface pattern. Supports multiple GPIO sources (Windows file-based, HTTP simulator) with a unified API.

## Features

- **Simple HAL Interface** - Just 4 functions: Init, Configure, Read, Write
- **Multiple Implementations** - Windows file-based, HTTP simulator
- **Pin Name-Based** - Use logical pin names (e.g., "LED1", "BUTTON1") instead of pin numbers
- **Hardware Agnostic** - Switch implementations without changing application code
- **Zero Dependencies** - Pure C, no external libraries required (except WinHTTP on Windows)

## Architecture

The GPIO driver uses a **self-contained implementation architecture** where each implementation (HTTP, Windows, STM32, ESP32, etc.) handles its own pin configuration internally. This makes implementations independent, extensible, and easy to switch with a single `#define`.

**Key Benefits:**
- ✅ **Self-Contained** - Each implementation auto-configures its pins during initialization
- ✅ **Easy Switching** - Change implementation by switching one `#define`
- ✅ **Platform Independent** - Application code remains the same across implementations
- ✅ **Extensible** - Each platform can have different pins and capabilities

See [`ARCHITECTURE.md`](ARCHITECTURE.md) for detailed architecture documentation.

## Quick Start

### 1. Include the library

```c
#include "gpioLib.h"

// Choose your implementation (switch with a single define):
#define GPIO_SOURCE_HTTP    // Use HTTP simulator
// #define GPIO_SOURCE_WINDOWS  // Use Windows file-based

#ifdef GPIO_SOURCE_HTTP
#include "implementations/gpioLib_http.h"
#elif defined(GPIO_SOURCE_WINDOWS)
#include "implementations/gpioLib_windows.h"
#endif
```

### 2. Register and use (Pins auto-configured!)

```c
int main(void)
{
    // Register GPIO interface
    #ifdef GPIO_SOURCE_HTTP
    vHalRegisterGpioInterface(&sGpioInterfaceHTTP);
    #elif defined(GPIO_SOURCE_WINDOWS)
    vHalRegisterGpioInterface(&sGpioInterfaceWindows);
    #endif
    
    // Initialize (pins are auto-configured internally!)
    const sGpioInterface_t *psGpio = psHalGetGpioInterface();
    if (psGpio && psGpio->vHalGpioInitFunc)
    {
        psGpio->vHalGpioInitFunc();  // LED1 and BUTTON1 auto-configured here!
    }
    
    // Use pins directly - no manual configuration needed!
    psGpio->eHalGpioWriteFunc("LED1", true);  // Turn ON
    
    bool bButtonState;
    psGpio->eHalGpioReadFunc("BUTTON1", &bButtonState);
    
    return 0;
}
```

**Note:** Pins are automatically configured by each implementation during initialization:
- **LED1** → Auto-configured as OUTPUT
- **BUTTON1** → Auto-configured as INPUT with pull-up

## Building

The GPIO driver supports a standard, cross-platform build system using both **Make** and **CMake**.

### Using Make (Recommended for simple CLI builds)

To build the GPIO example using the provided Makefile:

```bash
cd gpio_driver
# On Windows (MinGW), use:
mingw32-make
# On Linux/Mac or if 'make' is installed:
make
```

### Using CMake (Recommended for IDEs and hierarchical builds)

To build using CMake:

```bash
cd gpio_driver
mkdir build
cd build
cmake ..
cmake --build .
```

## Project Structure

```
gpio_driver/
├── gpioLib.h/c              # Main library interface
├── common.h                 # Common definitions
├── implementations/         # GPIO source implementations
│   ├── gpioLib_windows.c/h # Windows file-based (for testing)
│   └── gpioLib_http.c/h    # HTTP simulator
├── examples/
│   └── example_main.c       # Complete example
├── simulator/               # Python HTTP simulator
│   └── gpio_simulator.py
├── build.bat                # Windows build script
└── README.md
```

## Available Implementations

### Windows GPIO (`gpioLib_windows`)
- File-based GPIO simulation
- Stores pin states in `gpio_states/` directory
- Useful for testing on Windows without hardware
- Each pin state saved as `gpio_states/{pinName}.txt`

### HTTP Simulator (`gpioLib_http`)
- Fetches GPIO from Python HTTP simulator
- Useful for testing/simulation
- Requires simulator running on `localhost:8080`
- **Optimized Performance**: HTTP connection reuse (keep-alive) for near-instant response
- **Auto-configures**: LED1 (OUTPUT), BUTTON1 (INPUT with pull-up) on init

## GPIO Configuration

### Direction
- `GPIO_DIR_INPUT` - Pin configured as input
- `GPIO_DIR_OUTPUT` - Pin configured as output

### Pull Configuration
- `GPIO_PULL_NONE` - No pull-up/pull-down
- `GPIO_PULL_UP` - Internal pull-up resistor
- `GPIO_PULL_DOWN` - Internal pull-down resistor

## Testing with HTTP Simulator

### Basic Setup

1. Start the Python simulator:
```bash
cd simulator
python gpio_simulator.py
```

2. Use HTTP implementation in your code:
```c
#include "implementations/gpioLib_http.h"
vHalRegisterGpioInterface(&sGpioInterfaceHTTP);
```

### Interactive HTTP Control

The HTTP simulator provides **two ways** to interact with GPIO pins:

#### Method 1: Command-Line Interface (Recommended)

The simulator includes an interactive command-line interface for direct pin control:

1. Start the Python simulator:
```bash
cd simulator
python gpio_simulator.py
```

2. Use the interactive prompt:
```bash
GPIO> BUTTON1 0    # Set BUTTON1 to LOW (pressed)
GPIO> BUTTON1 1    # Set BUTTON1 to HIGH (released)
GPIO> status       # Show all pin states
GPIO> help         # Show available commands
GPIO> quit         # Exit simulator
```

3. Run your C application in another terminal:
```bash
cd gpio_driver
# Build (use mingw32-make on Windows)
make
# Run
.\build\example.exe
```

The C application will **automatically detect button changes** and toggle the LED in real-time with near-instant response (< 50ms latency)!

#### Method 2: HTTP POST Requests (For Automation)

You can also control pins via HTTP POST requests:

```bash
# Turn LED1 ON
curl -X POST http://localhost:8080/api/gpio/LED1 \
  -H "Content-Type: application/json" \
  -d '{"value":1}'

# Simulate BUTTON1 press (LOW = pressed with pull-up)
curl -X POST http://localhost:8080/api/gpio/BUTTON1 \
  -H "Content-Type: application/json" \
  -d '{"value":0}'
```

Or use the automated test script:
```bash
cd simulator
.\test_commands.bat
```

**Performance:** The HTTP implementation uses connection reuse (keep-alive) for optimal performance, resulting in sub-50ms response times for button-triggered LED toggles.

## API Reference

### Initialization
```c
void vHalRegisterGpioInterface(const sGpioInterface_t *psInterface);
const sGpioInterface_t *psHalGetGpioInterface(void);
```

### Pin Configuration
```c
eRetType_t eHalGpioConfigureFunc(const sGpioConfig_t *psConfig);
```

### Pin Operations
```c
eRetType_t eHalGpioReadFunc(const char *pcPinName, bool *pbValue);
eRetType_t eHalGpioWriteFunc(const char *pcPinName, bool bValue);
```

## Example Usage

The example demonstrates:
- **Self-contained initialization** - Pins auto-configured by implementation
- **Simple button monitoring** - Detect button state changes and toggle LED
- **Real-time response** - Fast polling with optimized HTTP connection reuse

Run the example:
```bash
cd gpio_driver
# Build (use mingw32-make on Windows)
make
# Run
.\build\example.exe
```

**Example Output:**
```
[BUTTON] State: HIGH -> LED: OFF
[BUTTON] State: LOW -> LED: ON
[BUTTON] State: HIGH -> LED: OFF
[BUTTON] State: LOW -> LED: ON
```

The example continuously monitors BUTTON1 and toggles LED1 whenever the button state changes, with near-instant response time.

## Performance Characteristics

- **HTTP Connection Reuse**: Persistent connections (keep-alive) eliminate connection overhead
- **Optimized Timeouts**: 100ms timeout for local network operations
- **Fast Polling**: 1ms polling interval for responsive button detection
- **Sub-50ms Latency**: Button press to LED response typically < 50ms
- **Retry Logic**: Automatic retries with short delays for transient network issues

## Related Documentation

- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Detailed architecture documentation for self-contained implementations
- **[../docs/DRIVER_ROADMAP.md](../docs/DRIVER_ROADMAP.md)** - Complete driver ecosystem roadmap and next driver proposals

## Driver Ecosystem

This GPIO driver is part of a larger HAL driver ecosystem:

- ✅ **GPIO Driver** (this driver) - Digital I/O with multiple implementations
- ✅ **Timing Driver** - Time management with multi-source support
- ✅ **Logging Driver** - Structured JSON logging
- 🔄 **Sensor Driver** - Recommended next driver (see [DRIVER_ROADMAP.md](../docs/DRIVER_ROADMAP.md))

## License

MIT License - see LICENSE file for details

