# Chat starter prompt – HAL Embedded MCP

Copy or @-mention this when you start a new chat so the agent has context.

---

**Context: HAL Embedded MCP**

- This repo has an **MCP server** (`hal_embedded_mcp/server/run_server.py`) that controls a real MCU (AVR) over **serial (COM3, 57600)**.
- **MCP tools:** `gpio_write` (pin_id, value), `gpio_read` (pin_id). Pins: **LED1**, **BUTTON1**.
- **COM port is exclusive:** Only one process can use COM3 at a time. If the hal-embedded MCP server is connected, it holds the port — do **not** run inline Python that opens COM3 (you will get “Access denied”). Use MCP tools only in that case.
- For **agent-driven complex or timed logic** (blinks, sequences, custom timing), use **inline Python** — but only when MCP is **not** using the port (e.g. hal-embedded disabled or not connected in this session):
  - `python -c "import serial, time; s=serial.Serial('COM3',57600,timeout=2); ...; s.close()"`
  - Same protocol: send lines like `gpio_write LED1 1\n`, `gpio_read LED1\n`.
- **MCU firmware:** build/flash from `hal_embedded_mcp/mcu/avr` (build.bat, flash.bat). Board must be connected and firmware flashed.

**Summary:** When MCP is on → use MCP tools only. When MCP is off → use `python -c "import serial...` for any logic (simple or complex). Do not mix both on the same port.
