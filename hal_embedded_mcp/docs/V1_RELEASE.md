# V1 release checklist

**HAL Embedded MCP v1.0.0** – Control real MCU GPIO from an MCP client (Cursor, Antigravity, etc.) or via inline Python.

## Supported

- **MCU:** AVR (ATmega328P / Arduino Nano); build with avr-gcc, flash with avrdude (PATH or documented env).
- **Host:** Python 3.9+; Windows, Linux, macOS.
- **MCP:** stdio transport; tools `gpio_write`, `gpio_read`. Tested with Cursor and Antigravity.
- **Serial:** One port (e.g. COM3), 57600 baud; configurable via env.

## Limitations

- **One process per port** – Either the MCP server uses the serial port, or an inline Python script does. Not both at once (inline will get "Access denied" if MCP is connected).
- **Pins** – Defined in `config/config.json`; default LED1, BUTTON1. Re-run codegen after changing config.
- **Resources** – MCP resources (device/config files) are not part of v1; tools only.

## Release contents

- [CHANGELOG.md](../CHANGELOG.md) – v1.0.0 notes
- [QUICKSTART.md](../QUICKSTART.md) – Get started in a few minutes
- [CHAT_PROMPT.md](../CHAT_PROMPT.md) – Prompt for new chats (MCP vs inline, port exclusive)
- [server/README.md](../server/README.md) – Server setup, codegen, CLI
- [mcu/avr/README.md](../mcu/avr/README.md) – Build, flash, toolchain

## Version

- Server: `__version__ = "1.0.0"` in `server/run_server.py`.
