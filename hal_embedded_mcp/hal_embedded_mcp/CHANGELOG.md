# Changelog

## [1.0.0] – v1 release

- **MCP server** – Python server exposes `gpio_write` and `gpio_read` over stdio; forwards commands to MCU over serial (UART).
- **AVR firmware** – Build and flash from `mcu/avr`; board executes line protocol (`gpio_write LED1 1`, `gpio_read LED1`) and responds with `OK`, `GPIO_READ <pin> <0|1>`, or `ERR`.
- **Serial** – Default COM3 (Windows) / `/dev/ttyUSB0` (Linux/macOS), 57600 baud; configurable via `HAL_MCP_SERIAL_PORT` and `HAL_MCP_SERIAL_BAUD`.
- **MCP clients** – Verified with Cursor and Antigravity; add hal-embedded server with command pointing at `run_server.py` (full path) and env for port/baud.
- **CLI** – `python -m server.run_server --cli` for interactive serial testing.
- **Inline scripting** – When MCP is not using the port, use `python -c "import serial; ..."` for complex or timed logic (see [CHAT_PROMPT.md](CHAT_PROMPT.md)).
- **Docs** – README, server/README, mcu/avr/README, QUICKSTART, CHAT_PROMPT, V1 release checklist.

**Limitation:** Only one process can use the serial port at a time (MCP server or inline script, not both).
