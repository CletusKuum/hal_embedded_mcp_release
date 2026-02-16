#!/usr/bin/env python3
"""
Example: complex blink pattern (no firmware change).

Blink LED1 10 times per round with doubling delay (base_delay, 2*base, 4*base, ...).
Repeat 10 rounds; each round uses a doubled starting delay (round 0: 2s, round 1: 4s, ...).

Run from hal_embedded_mcp (or with PYTHONPATH set so serial is available):
  set HAL_MCP_SERIAL_PORT=COM3
  set HAL_MCP_SERIAL_BAUD=57600
  python scripts/example_complex_blink.py

Ensure no other process (e.g. MCP server) is using the serial port.
"""
from __future__ import annotations

import os
import sys
import time

try:
    import serial
except ImportError:
    print("pip install pyserial", file=sys.stderr)
    sys.exit(1)

PORT = os.environ.get("HAL_MCP_SERIAL_PORT", "COM3" if sys.platform == "win32" else "/dev/ttyUSB0")
BAUD = int(os.environ.get("HAL_MCP_SERIAL_BAUD", "57600"))
BLINKS_PER_ROUND = 10
ROUNDS = 10
BASE_DELAY_SEC = 0.2


def send_cmd(ser: serial.Serial, line: str) -> None:
    ser.reset_input_buffer()
    ser.write((line.strip() + "\n").encode("utf-8"))
    ser.flush()
    time.sleep(0.08)


def main() -> None:
    print(f"Opening {PORT} at {BAUD}...")
    print(f"Pattern: {BLINKS_PER_ROUND} blinks per round (delay doubles each blink), "
          f"{ROUNDS} rounds (base delay doubles each round). Base = {BASE_DELAY_SEC}s.")
    with serial.Serial(PORT, BAUD, timeout=2.0) as ser:
        for round_idx in range(ROUNDS):
            base = BASE_DELAY_SEC * (2 ** round_idx)
            print(f"  Round {round_idx + 1}/{ROUNDS}: base delay = {base:.1f}s")
            delay = base
            for _ in range(BLINKS_PER_ROUND):
                send_cmd(ser, "gpio_write LED1 1")
                time.sleep(delay)
                send_cmd(ser, "gpio_write LED1 0")
                time.sleep(delay)
                delay *= 2
    print("Done.")


if __name__ == "__main__":
    main()
