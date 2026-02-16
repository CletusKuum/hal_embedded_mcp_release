# HAL Embedded MCP Server

**Embedded MCP (Model Context Protocol) – Fastest validation path**

Expose the HAL Drivers HTTP API and hardware abstraction as an MCP server so AI assistants (e.g. Claude Desktop) can discover and control hardware via tools and resources.

## Status

**v1.0.0** – Verified with AVR (Arduino Nano) and MCP clients (Cursor, Antigravity). Hardware controls work via tools and inline Python. See [CHANGELOG.md](CHANGELOG.md) and [docs/V1_RELEASE.md](docs/V1_RELEASE.md).

**Quick start:** [QUICKSTART.md](QUICKSTART.md) (flash MCU, install deps, run MCP or inline script).

## Goal

- Wrap the existing HAL Drivers in the MCP protocol.
- Expose HAL functions as **MCP tools** (`gpio_write`, `gpio_read`).
- Expose device/config files as **MCP resources**.
- Validate with Claude Desktop / Cursor and physical hardware.

## Dependencies

- Existing HTTP simulator: `unified_simulator/`
- HAL drivers and configs (GPIO, UART, sensor, etc.)
- MCP SDK (e.g. Python `mcp` package) for the server implementation

## Architecture

Design and component layout are in **[ARCHITECTURE.md](ARCHITECTURE.md)** (tools, resources, HAL backend, directory structure, phases).

## How to run

1. **MCU (AVR)** – Build and flash the firmware so the board listens on UART and executes MCP commands. See **[mcu/avr/README.md](mcu/avr/README.md)** (build.bat / build.sh, flash.bat / flash.sh, serial test).
2. **MCP server (Python)** – Run the server so your MCP client (e.g. Claude Desktop, Cursor) can call `gpio_write` / `gpio_read`; the server forwards them to the MCU over serial. See **[server/README.md](server/README.md)** for:
   - Generating `server/generated/mcp_schema.py` from `config/config.json`
   - Installing dependencies (`pip install -r server/requirements.txt`)
   - Setting `HAL_MCP_SERIAL_PORT` and `HAL_MCP_SERIAL_BAUD` if needed
   - Starting the server: from `hal_embedded_mcp`, run `python -m server.run_server` (stdio transport)
   - Configuring your MCP client to launch this server (command, full path to `run_server.py`, env)

**Limitation:** Only one process can use the serial port at a time (MCP server or inline `python -c "import serial; ..."`, not both). See [CHAT_PROMPT.md](CHAT_PROMPT.md).

## Roadmap

See [docs/ROADMAP.md](../docs/ROADMAP.md) for overall progress across Core Products.

---

*Priority: ⭐⭐⭐⭐⭐ (Highest – Fastest Validation)*
