# HAL Embedded MCP Server

**Embedded MCP (Model Context Protocol) – Fastest validation path**

Expose the HAL Drivers HTTP API and hardware abstraction as an MCP server so AI assistants (e.g. Claude Desktop) can discover and control hardware via tools and resources.

## Status

**v1.0.0** – Verified with AVR (Arduino Nano) and MCP clients (Cursor, Antigravity). Hardware controls work via tools and inline Python. See [CHANGELOG.md](CHANGELOG.md) and [docs/V1_RELEASE.md](docs/V1_RELEASE.md).

**Quick start:** [QUICKSTART.md](QUICKSTART.md) (flash MCU, install deps, run MCP or inline script).

## What problem this solves

Changing timing or sequences usually means editing firmware, rebuilding, reflashing, and debugging over serial. That slows down try-and-see and demos.

Here you keep **one generic firmware** (a small command executor). Complex sequences and timing live on the **host** (Python + MCP or inline scripts). You describe behavior in natural language or run a script; **no C change, no reflash** for those changes. Same HAL and protocol can target AVR, PC simulation, or STM32. Result: **faster iteration on behavior and demos** without firmware churn.

## Stack at a glance

```
  You / AI     →   MCP client (Cursor, etc.)   →   Python server   →   UART   →   MCU (HAL)   →   GPIO / hardware
                       (tools: gpio_write, gpio_read)                 serial      (one command at a time)
```

So it’s not “MCU + Python glued together” — it’s a defined path: **MCP ↔ server ↔ serial ↔ MCU ↔ HAL ↔ platform**.

**Not a general-purpose HAL.** This is a minimal stack to control MCU GPIO from MCP clients and iterate on behavior without reflashing. For a broad hardware-abstraction library, see vendor HALs (e.g. STM32Cube) or cross-platform ecosystems.

## Features

- **Config-driven tools** – Pin names and tool list come from `config.json`; the server and MCU stay in sync via codegen. The AI only sees valid pins (e.g. `LED1`, `BUTTON1`).
- **Validation before hardware** – The server rejects unknown `pin_id` and invalid inputs before sending anything to the MCU. Tool calls return `ERR unknown pin` or the MCU response; no raw register writes from the AI.
- **Error handling** – The server returns MCU responses (`OK`, `GPIO_READ <pin> <0|1>`, `ERR ...`) or a clear message if the MCU does not respond (e.g. port in use, board not connected).

## Supported hardware (v1)

| Component      | Protocol        | Status   |
|----------------|-----------------|----------|
| GPIO (digital)| UART line cmd   | Working  |
| UART (serial) | Host to MCU     | Working  |
| I2C / ADC     | —               | Not in v1|

Pins and directions are defined in `config/config.json`; the MCU firmware validates pin names before executing.

## Safety / validation

The MCP server acts as a **guardrail layer**: it only forwards tool calls whose parameters pass validation. For `gpio_write`, `pin_id` must be in the generated pin list (from config); `value` is a boolean (high/low). For `gpio_read`, `pin_id` must be valid. Invalid calls get an immediate `ERR` response and are never sent to the MCU. The firmware also validates commands and pins before touching hardware. See [docs/SAFETY_AND_VALIDATION.md](docs/SAFETY_AND_VALIDATION.md) for details.

## Example: tool definition

Tools are defined in Python and exposed to any MCP client (Cursor, Claude Desktop, etc.):

```python
@mcp.tool()
def gpio_write(pin_id: str, value: bool) -> str:
    """Set a GPIO pin high (True) or low (False). pin_id must be one of the configured pins."""
    if pin_id not in MCP_PIN_NAMES:
        return f"ERR unknown pin. Allowed: {', '.join(MCP_PIN_NAMES)}"
    return _send_cmd(f"gpio_write {pin_id} {1 if value else 0}")
```

Pin names (`MCP_PIN_NAMES`) are generated from `config.json`, so adding a new pin is a config change and codegen run, not hand-edited tool code.

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

## Examples: natural language instead of complex firmware

You don’t need to write (or reflash) firmware for tricky timing. Describe the behavior in natural language; the AI runs it via MCP or an inline script. See **[docs/NATURAL_LANGUAGE_EXAMPLES.md](docs/NATURAL_LANGUAGE_EXAMPLES.md)** for:

- *“Blink 5 times with 1s delay”* → AI runs a short script.
- *“Blink 10 times with 2s delay, double the delay after each blink; repeat that whole pattern 10 times, doubling the starting delay each round”* → would be a state-machine in C; here the AI runs [scripts/example_complex_blink.py](scripts/example_complex_blink.py) or equivalent.

Run the reference script (with MCP server **off** so the port is free):  
`python scripts/example_complex_blink.py`

## Roadmap

See [docs/ROADMAP.md](../docs/ROADMAP.md) for overall progress across Core Products.

---

*Priority: ⭐⭐⭐⭐⭐ (Highest – Fastest Validation)*
