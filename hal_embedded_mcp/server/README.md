# HAL Embedded MCP Server

Python MCP server that exposes **gpio_write** and **gpio_read** as tools. It talks to the MCU over **serial (UART)** and uses the generated schema from `config.json`.

## Prerequisites

1. **MCU firmware** must be flashed and connected (see [mcu/avr/README.md](../mcu/avr/README.md)). Serial port must be free (close any serial monitor before starting the server).
2. **Python 3.9+**
3. **Generated schema** – generate once (or after changing `config.json`):

   From repo root:
   ```bash
   python hal_embedded_mcp/scripts/gen_mcp_from_config.py hal_embedded_mcp/config/config.json --python-out hal_embedded_mcp/server/generated/
   ```
   Or from `hal_embedded_mcp`:
   ```bash
   python scripts/gen_mcp_from_config.py config/config.json --python-out server/generated/
   ```
   This creates/updates `server/generated/mcp_schema.py` (pin names and tool list).

## Install dependencies

From this directory or repo root:

```bash
pip install -r hal_embedded_mcp/server/requirements.txt
```

Or:

```bash
pip install mcp pyserial
```

## Configure serial port

By default the server uses:

- **Windows**: `COM3`
- **Linux / macOS**: `/dev/ttyUSB0`
- **Baud**: `57600` (aligned with MCU)

Override with environment variables:

```bash
set HAL_MCP_SERIAL_PORT=COM3
set HAL_MCP_SERIAL_BAUD=57600
python -m server.run_server
```

## Start the server

### 1. MCP Mode (Stdio)
Run from the **hal_embedded_mcp** directory. This is for use by AI clients (Claude Desktop, etc.):

```bash
python -m server.run_server
```

### 2. CLI Mode (Manual Testing)
If you want to test hardware commands directly in your terminal:

```bash
python -m server.run_server --cli
```
This opens an interactive shell where you can type `gpio_write LED1 1` or `gpio_read BUTTON1` directly.

> [!NOTE]
> The server uses a **persistent serial connection**. It stays connected to the MCU, which prevents the hardware from resetting between commands (no loss of GPIO state).

## Using with an MCP client

1. Flash the MCU and connect it (one serial port).
2. Generate the schema (see above) and install dependencies.
3. In your MCP client config, add a server that runs this script with stdio.

   **Example (Claude Desktop / Cursor)** – in the config file that lists MCP servers, add something like:

   ```json
   {
     "mcpServers": {
       "hal-embedded": {
         "command": "python",
         "args": ["-m", "server.run_server"],
         "cwd": "C:/path/to/hal_drivers/hal_embedded_mcp",
         "env": {
           "HAL_MCP_SERIAL_PORT": "COM3",
           "HAL_MCP_SERIAL_BAUD": "57600"
         }
       }
     }
   }
   ```

   Adjust `cwd` to your `hal_embedded_mcp` folder and `HAL_MCP_SERIAL_PORT` to your MCU’s port.

4. Restart the client; it should list tools **gpio_write** and **gpio_read**. Invoking them sends a line to the MCU over serial and returns the MCU’s response.

## Tools

| Tool         | Parameters              | Effect                          |
|-------------|--------------------------|---------------------------------|
| **gpio_write** | `pin_id` (e.g. LED1), `value` (bool) | Sends `gpio_write LED1 1` (or 0) to MCU |
| **gpio_read**  | `pin_id` (e.g. BUTTON1)  | Sends `gpio_read BUTTON1`, returns MCU response |

Pin names come from `config/config.json` and the generated `mcp_schema.py`.
