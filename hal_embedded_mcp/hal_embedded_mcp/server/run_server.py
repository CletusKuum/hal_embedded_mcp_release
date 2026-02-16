#!/usr/bin/env python3
"""
HAL Embedded MCP Server – exposes gpio_write / gpio_read as MCP tools.
Sends commands to the MCU over serial; uses server/generated/mcp_schema.py (from config.json).
Run from repo root or hal_embedded_mcp: python -m server.run_server
Or: python server/run_server.py (with hal_embedded_mcp as cwd so generated/ is found).
"""
from __future__ import annotations

__version__ = "1.0.0"

import os
import sys
import time

# Allow importing generated schema when run as script or -m
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
if _SCRIPT_DIR not in sys.path:
    sys.path.insert(0, _SCRIPT_DIR)

try:
    from generated.mcp_schema import MCP_PIN_NAMES, MCP_TOOLS
except ImportError:
    print("Error: generated/mcp_schema.py not found. Run codegen first:", file=sys.stderr)
    print("  python hal_embedded_mcp/scripts/gen_mcp_from_config.py config/config.json --python-out server/generated/", file=sys.stderr)
    sys.exit(1)

try:
    import serial
except ImportError:
    print("Error: pyserial not installed. pip install pyserial", file=sys.stderr)
    sys.exit(1)

# MCP SDK
try:
    from mcp.server.fastmcp import FastMCP
except ImportError:
    print("Error: mcp package not installed. pip install mcp", file=sys.stderr)
    sys.exit(1)

SERIAL_PORT = os.environ.get("HAL_MCP_SERIAL_PORT", "COM3" if sys.platform == "win32" else "/dev/ttyUSB0")
SERIAL_BAUD = int(os.environ.get("HAL_MCP_SERIAL_BAUD", "57600"))
DEBUG_SERIAL = "--debug-serial" in sys.argv

mcp = FastMCP("HAL Embedded MCP")

# Global serial instance
_serial_conn: serial.Serial | None = None


def _debug_drain(ser: serial.Serial) -> None:
    """Read any remaining bytes for a short time and print for debug."""
    ser.timeout = 0.2
    collected = []
    for _ in range(25):
        b = ser.read(64)
        if b:
            collected.append(b)
    if collected:
        print("[debug] late bytes:", b"".join(collected).__repr__(), file=sys.stderr)
    ser.timeout = 2.0


def get_serial() -> serial.Serial:
    """Get or create the persistent serial connection."""
    global _serial_conn
    if _serial_conn is None or not _serial_conn.is_open:
        _serial_conn = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=2.0)
        # Give MCU time to boot after possible DTR reset on first open
        time.sleep(2.0)
    return _serial_conn


def _send_cmd(line: str) -> str:
    """Send one line to MCU and return response; interpret success/failure.
    Drains RX buffer before sending, then waits briefly for MCU to process
    and monitors for one line within the serial timeout window."""
    try:
        ser = get_serial()
        ser.reset_input_buffer()
        ser.write((line.strip() + "\n").encode("utf-8"))
        ser.flush()
        time.sleep(0.08)
        raw = ser.readline()
        if not raw and ser.in_waiting == 0:
            time.sleep(0.12)
            raw = ser.readline()
        resp = raw.decode("utf-8", errors="replace").strip()
        if DEBUG_SERIAL:
            print(f"[debug] raw bytes: {raw!r}", file=sys.stderr)
    except Exception as e:
        return f"ERR: {e}"
    if not resp:
        if DEBUG_SERIAL:
            _debug_drain(ser)
        return (
            "No response from MCU. Check: board connected, correct port "
            f"({SERIAL_PORT}), firmware flashed, and no other app using the port."
        )
    if resp.upper() == "OK":
        return "OK"
    if resp.upper().startswith("ERR"):
        return resp
    if resp.upper().startswith("GPIO_READ"):
        return resp
    return resp


@mcp.tool()
def gpio_write(pin_id: str, value: bool) -> str:
    """Set a GPIO pin high (True) or low (False). pin_id must be one of the configured pins."""
    if pin_id not in MCP_PIN_NAMES:
        return f"ERR unknown pin. Allowed: {', '.join(MCP_PIN_NAMES)}"
    return _send_cmd(f"gpio_write {pin_id} {1 if value else 0}")


@mcp.tool()
def gpio_read(pin_id: str) -> str:
    """Read a GPIO pin value (0 or 1). pin_id must be one of the configured pins."""
    if pin_id not in MCP_PIN_NAMES:
        return f"ERR unknown pin. Allowed: {', '.join(MCP_PIN_NAMES)}"
    return _send_cmd(f"gpio_read {pin_id}")


def run_cli():
    """Simple interactive CLI for manual testing of the serial link."""
    print(f"--- HAL MCP CLI Mode (Port: {SERIAL_PORT}, Baud: {SERIAL_BAUD}) ---")
    if DEBUG_SERIAL:
        print("Serial debug: ON (raw/late bytes printed to stderr)", file=sys.stderr)
    print(f"Allowed pins: {', '.join(MCP_PIN_NAMES)}")
    print("Commands: gpio_write <pin> <0/1>, gpio_read <pin>, quit")
    while True:
        try:
            line = input("> ").strip()
            if not line or line.lower() in ["quit", "exit"]:
                break
            result = _send_cmd(line)
            if result.startswith("ERR") or "No response" in result:
                print(f"Error: {result}")
            else:
                print(f"MCU Response: {result}")
        except EOFError:
            break
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    if "--cli" in sys.argv:
        run_cli()
    else:
        mcp.run(transport="stdio")
