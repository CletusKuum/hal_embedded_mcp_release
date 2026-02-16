# HAL Embedded MCP – Quick start (about 5 minutes)

Get from zero to controlling an LED via MCP tools or inline Python.

## 1. Hardware

- **Board:** AVR (e.g. Arduino Nano / ATmega328P) with LED and optional button.
- **Connection:** USB to PC (shows as COM3 on Windows or `/dev/ttyUSB0` on Linux/macOS).

## 2. Flash the MCU

From the repo:

```bash
cd hal_embedded_mcp/mcu/avr
./build.bat
./flash.bat
```

(Use `build.sh` / `flash.sh` on Linux/macOS; set `PORT` and `BAUD` if needed. AVR toolchain must be on PATH – see [mcu/avr/README.md](mcu/avr/README.md).)

## 3. Python setup

From `hal_embedded_mcp` (or repo root):

```bash
pip install -r server/requirements.txt
python scripts/gen_mcp_from_config.py config/config.json --python-out server/generated/
```

Set port/baud if not using defaults:

- **Windows:** `set HAL_MCP_SERIAL_PORT=COM3` and `set HAL_MCP_SERIAL_BAUD=57600`
- **Linux/macOS:** `export HAL_MCP_SERIAL_PORT=/dev/ttyUSB0`

## 4a. Use MCP (Cursor / Antigravity)

1. Add the **hal-embedded** MCP server in your client config (e.g. Cursor MCP or `mcp.json`).
2. Command: `python`
3. Args: full path to `hal_embedded_mcp/server/run_server.py`
4. Env: `HAL_MCP_SERIAL_PORT`, `HAL_MCP_SERIAL_BAUD` if needed.
5. Restart the client; you should see tools **gpio_write** and **gpio_read**. Invoke them (e.g. turn LED1 on/off).

## 4b. Use inline Python (no MCP server)

When the MCP server is **not** running (so it’s not holding the port), you can drive the board with a one-liner:

```bash
python -c "import serial; s=serial.Serial('COM3',57600,timeout=2); s.write(b'gpio_write LED1 1\n'); s.flush(); s.close(); print('LED1 on.')"
```

Change `COM3` to your port. For blinks or sequences, see [CHAT_PROMPT.md](CHAT_PROMPT.md).

## 5. Sanity check

- **CLI:** `python -m server.run_server --cli` then type `gpio_write LED1 1` and `gpio_write LED1 0` (ensure no other app is using the port).
- **Inline:** Run the one-liner above; the LED should turn on.

---

**Pins:** Default config exposes **LED1** and **BUTTON1**. Edit `config/config.json` and re-run the codegen script to change pins or add more.

**Next:** See [docs/NATURAL_LANGUAGE_EXAMPLES.md](docs/NATURAL_LANGUAGE_EXAMPLES.md) for complex sequences (e.g. “blink 10 times with doubling delay, repeat 10 rounds”) that you describe in natural language instead of writing firmware. Run `python scripts/example_complex_blink.py` (port free) to try the reference example.
