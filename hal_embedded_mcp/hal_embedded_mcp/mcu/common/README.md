# MCU common

Shared application code for all HAL Embedded MCP platforms. **Main** lives here and talks to `tool_registry` / `tool_handlers_gpio`; platform is selected at build time.

- **`app_main.c`** – `vAppInit()` / `vAppLoop()` and `main()`. On AVR, registers a UART line callback so that every received line is dispatched by main: JSON → Digital Twin path (`vApplyReceivedJsonLine`), non-JSON → MCP (`vMcpHandleLine`).
- Other platforms (STM32, PC) use the same app; when they gain a UART (or other) line API, they can expose a similar callback so main keeps doing the dispatch.

Platform-specific builds:

- **avr/** – AVR toolchain; uses `gpio_driver/implementations/avr` and UART line callback from `uart_line_callback.h`.
- **stm32/** – ARM toolchain; uses `gpio_driver/implementations/stm32`; UART callback for MCP TBD.
- **pc/** – Host build (HTTP or WINDOWS); uses `gpio_driver/implementations/pc`; no UART callback (MCP via other transport later).
