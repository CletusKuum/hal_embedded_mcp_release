# VS Code Debug Configuration for GPIO Driver

This directory contains VS Code configuration files for debugging the GPIO driver example application.

## Files

- **`launch.json`**: Debug configurations
- **`tasks.json`**: Build tasks
- **`c_cpp_properties.json`**: IntelliSense configuration for C/C++

## Debug Configurations

### 1. Debug GPIO Example (HTTP)
Main debugging configuration for the HTTP-based GPIO example. This will:
- Build with debug symbols using `build-debug.bat`
- Launch the example executable
- Allow setting breakpoints and inspecting variables

### 2. Debug GPIO Example (Windows File-based)
Same as above but for Windows file-based GPIO implementation (if you switch the `#define GPIO_SOURCE` in `example_main.c`).

### 3. Debug GPIO Example (Attach to Process)
Allows attaching the debugger to an already-running `example.exe` process. Useful if you want to start the program manually first.

## Usage

### Starting Debug Session

1. **Make sure the simulator is running** (if using HTTP mode):
   ```bash
   cd simulator
   python gpio_simulator.py
   ```

2. **Set breakpoints** in your code by clicking in the gutter (left of line numbers)

3. **Press F5** or:
   - Go to Run and Debug (Ctrl+Shift+D)
   - Select "Debug GPIO Example (HTTP)" from dropdown
   - Click the green play button

4. **Debug controls**:
   - **F5**: Continue
   - **F10**: Step Over
   - **F11**: Step Into
   - **Shift+F11**: Step Out
   - **Shift+F5**: Stop

### Debugging Tips

- **Variables panel**: View local variables and their values
- **Watch panel**: Add expressions to monitor
- **Call Stack**: See the execution path
- **Debug Console**: Execute expressions in the current context

### Common Breakpoints

Good places to set breakpoints for GPIO debugging:
- `example_main.c`: Line ~164 - Button read loop
- `example_main.c`: Line ~178 - Button press detection
- `example_main.c`: Line ~182 - LED write
- `gpioLib_http.c`: Line ~263 - HTTP request timeout setting
- `gpioLib_http.c`: Line ~329 - HTTP send request
- `gpioLib_http.c`: Line ~200-210 - HTTP read response

## Building with Debug Symbols

The debug configuration automatically builds with debug symbols using `build-debug.bat`, which:
- Adds `-g` flag for GDB debug symbols
- Uses `-O0` (no optimization) for better debugging experience
- Includes all source files with debug information

You can also build manually:
```bash
build-debug.bat
```

## Requirements

- **GDB**: Should be installed with MinGW (usually at `C:\msys64\ucrt64\bin\gdb.exe`)
- **MinGW GCC**: Compiler should be in PATH
- **C/C++ Extension**: Install "C/C++" extension by Microsoft in VS Code

## Troubleshooting

### GDB not found
If you get "gdb not found", ensure MinGW is in your PATH:
- Check: `gdb --version` in terminal
- Update `launch.json` with full path: `"miDebuggerPath": "C:/msys64/ucrt64/bin/gdb.exe"`

### Breakpoints not working
- Ensure you built with `build-debug.bat` (includes `-g` flag)
- Check that source files match the executable
- Rebuild if you modified source files

### Can't see variables
- Make sure optimization is disabled (`-O0` in build-debug.bat)
- Variables might be optimized away in release builds

### Simulator connection issues during debugging
- Start the simulator in a separate terminal before starting debug session
- The debugger will pause at breakpoints, which might cause timeouts
- Consider using conditional breakpoints for HTTP operations

