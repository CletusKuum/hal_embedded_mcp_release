# HAL Embedded MCP – Standalone release (v1.0.0)

This folder is a **self-contained release**. You can zip and ship it as-is; no need for the full `hal_drivers` repo.

## What’s inside

| Folder | Purpose |
|--------|--------|
| **hal_embedded_mcp** | MCP server (Python), MCU firmware (AVR), config, scripts, docs. **Start here.** |
| gpio_driver | Required for building the AVR firmware (do not remove). |
| logging_driver | Required for building the AVR firmware (do not remove). |
| helper_utils | Required for building the AVR firmware (do not remove). |

## Quick start

1. **Flash the MCU**  
   `hal_embedded_mcp\mcu\avr\build.bat` then `flash.bat` (AVR toolchain on PATH).

2. **Run the MCP server**  
   From `hal_embedded_mcp`: install deps, run codegen, then `python -m server.run_server` or add to Cursor MCP.

Full steps: **[hal_embedded_mcp/QUICKSTART.md](hal_embedded_mcp/QUICKSTART.md)**.

## Version

v1.0.0 – See [hal_embedded_mcp/CHANGELOG.md](hal_embedded_mcp/CHANGELOG.md).
