# HAL Embedded MCP – Standalone release (v1.0.0)

Control **real MCU GPIO** from MCP clients (Cursor, Antigravity, Claude, etc.) via a Python server and serial. The firmware uses a **small HAL** so the same application layer can target AVR, PC simulator, or STM32; this release ships the minimal set for **MCP + AVR**.

**What this is:** An embedded stack that bridges **Model Context Protocol (MCP)** on the host and **hardware on the MCU**. The Python server exposes tools (`gpio_write`, `gpio_read`); it forwards tool calls over UART to the MCU, which runs the HAL and platform drivers. So you get **AI-driven hardware control** with a clear separation: MCP ↔ server ↔ serial ↔ MCU ↔ HAL ↔ platform (AVR/PC/STM32).

This folder is **self-contained**. Zip and ship it as-is; no need for the full `hal_drivers` repo.

### What problem this solves

Changing timing or sequences usually means editing firmware, rebuilding, reflashing, and debugging. Here you keep **one generic firmware** (command executor); complex sequences live on the **host** (Python + MCP or scripts). Describe behavior in natural language or run a script — **no C change, no reflash**. Result: **faster iteration on behavior and demos**. See [hal_embedded_mcp/docs/OVERVIEW.md](hal_embedded_mcp/docs/OVERVIEW.md).

### Stack at a glance

```
  You / AI  →  MCP client  →  Python server  →  UART  →  MCU (HAL)  →  GPIO / hardware
```

---

## What's inside

| Folder | Purpose |
|--------|--------|
| **hal_embedded_mcp** | MCP server (Python), MCU firmware (AVR), config, scripts, docs. **Start here.** |
| gpio_driver | HAL and platform drivers; required to build the AVR firmware (do not remove). |
| logging_driver | Logging used by the firmware build (do not remove). |
| helper_utils | Helpers used by the firmware build (do not remove). |

**Architecture and data flow:** [hal_embedded_mcp/ARCHITECTURE.md](hal_embedded_mcp/ARCHITECTURE.md).  
**Why this exists:** [hal_embedded_mcp/docs/OVERVIEW.md](hal_embedded_mcp/docs/OVERVIEW.md).

---

## Quick start

1. **Flash the MCU**  
   `hal_embedded_mcp\mcu\avr\build.bat` then `flash.bat` (AVR toolchain on PATH).

2. **Run the MCP server**  
   From `hal_embedded_mcp`: install deps, run codegen, then `python -m server.run_server` or add to Cursor MCP.

Full steps: **[hal_embedded_mcp/QUICKSTART.md](hal_embedded_mcp/QUICKSTART.md)**.

---

## Version

v1.0.0 – See [hal_embedded_mcp/CHANGELOG.md](hal_embedded_mcp/CHANGELOG.md).
