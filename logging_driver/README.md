# Logging Library - Production-Ready Optimized Implementation

A single, expert-level logging library for embedded systems with structured JSON output, zero-cost compile-time filtering, and production-ready features.

## Features

- **Single Optimized Implementation** - One unified codebase, no multiple implementations to choose from
- **Structured JSON Logging** - Machine-readable JSON with automatic metadata (RFC 3339 timestamps, source location, error codes)
- **Zero-Cost When Disabled** - Compile-time log level filtering eliminates runtime overhead
- **Efficient Buffer Management** - Stack-based buffers, no dynamic allocations
- **Thread-Safe Option** - Optional mutex protection for multi-threaded applications
- **Production-Ready** - Error code integration, build version tracking, proper escaping
- **Hardware Agnostic** - Works on any platform (console, file, HTTP output modes)

## Quick Start

```c
#include "logLib.h"

// Define layer constants
#define PROTOCOL_LAYER "PROTOCOL_LAYER"
#define GPIO_LAYER "GPIO_LAYER"

int main(void)
{
    // Initialize with configuration
    sLogConfig_t sConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_CONSOLE,
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    eLogInit(&sConfig);
    
    // Structured logging with automatic metadata
    LOG_ERROR(PROTOCOL_LAYER, RET_TYPE_NULL_POINTER, "Error: %s", error_msg);
    LOG_WARNING(GPIO_LAYER, RET_TYPE_FAIL, "Pin %d failed", pin);
    LOG_INFO(PROTOCOL_LAYER, RET_TYPE_SUCCESS, "Operation completed");
    LOG_DEBUG("Debug info: %d", value);
    
    vLogCleanup();
    return 0;
}
```

## Building

The library uses a modular build system. You can build it using **Make** or **CMake**.

### Using Make (Recommended for CLI)

**On Windows (MinGW):**
```bash
mingw32-make
```

**On Linux/Mac:**
```bash
make
```

### Using CMake (Recommended for IDEs)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Outputs
- **Executable:** `build/example.exe` (on Windows) or `build/example` (on Linux/Mac).
- **Static Library:** `build/logging_lib.a` (available via CMake).

## Output Format

All logs (ERROR, WARNING, INFO, DEBUG) output valid JSON in a pretty-printed, human-readable format:

```json
{
  "timestamp": "2025-12-10T17:14:55Z",
  "log_level": "ERROR",
  "message": "Null pointer in function eProcessMessage",
  "layer": "PROTOCOL_LAYER",
  "source": {
    "file": "src/protocolLayer/sessionManager.c",
    "function": "eProcessMessage",
    "line": 856
  },
  "build_version": "1.0.0",
  "error_code": 2,
  "error_description": "Null pointer error"
}
```

**Format Options:**
- **Pretty-printed JSON** (default): Multi-line, indented JSON format as shown above - valid JSON, easy to read
- **Compact JSON**: Single-line JSON format (set `LOG_LIB_FORMAT_STYLE 0`)

The format is valid JSON, fully machine-parseable, and includes all structured metadata.

## Configuration Options

### Compile-Time Optimizations

```c
// Disable JSON formatting (plain text output)
#define LOG_LIB_ENABLE_JSON 0

// Output format style: 0 = compact JSON, 1 = pretty-printed (default)
#define LOG_LIB_FORMAT_STYLE 1

// Compile-time log level filtering (zero runtime cost)
#define LOG_LIB_COMPILE_TIME_LEVEL LOG_LEVEL_INFO

// Adjust buffer sizes
#define LOG_LIB_MAX_MESSAGE_SIZE 1024
#define LOG_LIB_MAX_JSON_SIZE 2048
```

### Runtime Configuration

```c
sLogConfig_t sConfig = {
    .eLevel = LOG_LEVEL_INFO,           // Minimum log level
    .eMode = LOG_MODE_CONSOLE,          // Output mode: CONSOLE, FILE, HTTP
    .pcFilePath = "app.log",            // For LOG_MODE_FILE
    .pcHttpEndpoint = "http://...",     // For LOG_MODE_HTTP
    .pcBuildVersion = "1.0.0",          // Build version string
    .bThreadSafe = true                 // Enable thread safety
};

eLogInit(&sConfig);
```

## Output Modes

### Console Mode (Default)

```c
sConfig.eMode = LOG_MODE_CONSOLE;
```

Outputs JSON to stderr. Best for development and debugging.

### File Mode

```c
sConfig.eMode = LOG_MODE_FILE;
sConfig.pcFilePath = "/var/log/app.log";
```

Writes JSON logs to file. Best for production systems with persistent storage.

### HTTP Mode

```c
sConfig.eMode = LOG_MODE_HTTP;
sConfig.pcHttpEndpoint = "http://log-server:8080/api/logs";
```

Sends JSON logs via HTTP POST. Best for centralized log collection.

## Performance Optimizations

### 1. Compile-Time Filtering

```c
// Zero runtime cost - logs below INFO are completely eliminated at compile time
#define LOG_LIB_COMPILE_TIME_LEVEL LOG_LEVEL_INFO
```

When enabled, logs below the specified level are completely removed by the compiler (zero runtime overhead).

### 2. Fast Path Level Check

All logging macros check the log level first before any formatting work:

```c
// This check happens first (fast path)
if (eLevel > g_eCurrentLogLevel) {
    return;  // Exit immediately, no formatting
}
```

### 3. Stack-Based Buffers

No dynamic memory allocation. All buffers are stack-allocated for maximum performance.

### 4. Efficient JSON Escaping

Optimized string escaping for JSON output with minimal overhead.

## Thread Safety

Enable thread safety when needed:

```c
sConfig.bThreadSafe = true;
```

Uses platform-specific mutexes (Windows: `CRITICAL_SECTION`, POSIX: `pthread_mutex_t`).

## Use Cases

### Embedded Development on PC

```c
sLogConfig_t sConfig = {
    .eLevel = LOG_LEVEL_DEBUG,
    .eMode = LOG_MODE_CONSOLE,
    .pcBuildVersion = "dev-1.0.0",
    .bThreadSafe = false
};
```

### Production Deployment

```c
sLogConfig_t sConfig = {
    .eLevel = LOG_LEVEL_INFO,  // No debug logs in production
    .eMode = LOG_MODE_FILE,
    .pcFilePath = "/var/log/app.log",
    .pcBuildVersion = "1.0.0",
    .bThreadSafe = true
};
```

### Centralized Logging

```c
sLogConfig_t sConfig = {
    .eLevel = LOG_LEVEL_INFO,
    .eMode = LOG_MODE_HTTP,
    .pcHttpEndpoint = "http://log-aggregator:8080/api/logs",
    .pcBuildVersion = "1.0.0",
    .bThreadSafe = true
};
```

## API Reference

### Initialization

```c
eRetType_t eLogInit(const sLogConfig_t *psConfig);
void vLogCleanup(void);
```

### Log Level Management

```c
void vLogSetLevel(eLogLevel_t eLevel);
eLogLevel_t eLogGetLevel(void);
```

### Build Version

```c
void vLogSetBuildVersion(const char *pcVersion);
```

### Logging Macros

```c
LOG_ERROR(layer, errorCode, format, ...)
LOG_WARNING(layer, errorCode, format, ...)
LOG_INFO(layer, errorCode, format, ...)
LOG_DEBUG(format, ...)  // No layer/error code for debug
```

## Integration with Other Libraries

### With Timing Library

```c
#include "timeLib.h"
#include "logLib.h"

uint64_t u64Time = psTimeInterface->u64HalGetTimeMillisecFunc();
LOG_INFO(TIMER_LAYER, RET_TYPE_SUCCESS, "Event at %llu ms", u64Time);
```

## Memory Usage

- **Stack buffers**: ~2KB per log call (temporary, reclaimed immediately)
- **Static data**: ~1KB (configuration, error mappings)
- **No heap allocations**: Zero dynamic memory usage

Perfect for embedded systems with limited memory.

## Limitations

- HTTP mode uses `system()` call (requires curl on system)
- File mode appends indefinitely (no automatic rotation)
- JSON output size limited by `LOG_LIB_MAX_JSON_SIZE` (default 1024 bytes)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

MIT License - see [LICENSE](LICENSE) file.

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.
