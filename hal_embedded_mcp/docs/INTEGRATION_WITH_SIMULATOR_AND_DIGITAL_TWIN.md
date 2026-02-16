# Integration with Unified Simulator and Digital Twin

This doc summarizes the **current working flow** (gpio_driver + unified_simulator + hal_digital_twin_os) and the **proposed change**: one new endpoint on the simulator so MCP and digital twin share the same UART, with main and build driven by MCP config generation.

---

## Summary in one sentence

- **MCU side**: We **reproduce** the gpio_driver/examples setup (same build pattern, same use of gpio_driver/implementations) and update **main** with minimal modification: add MCP command handling (registry, parse command lines, dispatch, response) while still supporting **all gpio_driver/implementations** (AVR, STM32, HTTP, etc.) and the existing digital-twin state sync.
- **Simulator side**: We **update** the unified_simulator to **extract** MCP server commands (received via a new HTTP endpoint) and **redirect** them to the MCU over serial (TX); the simulator still reads serial from the MCU (RX) and updates state so the **digital twin keeps working** unchanged.

---

## 1. Current Working Flow

### 1.1 MCU (gpio_driver/examples/avr + common/app_main.c)

- **Build**: `config.json` → `scripts/gen_config.py` → `gpio_config_gen.c`; CMake compiles it with `app_main.c`, `platform_adapter.c`, `gpio_helper.c`, etc.
- **Runtime**:
  - **TX (MCU → Simulator)**: When `eGpioHelperWrite("LED1", true)` runs, `gpio_helper.c` calls `vHelperSend("GPIO", "LED1", 1)` → `printf("{\"t\":\"GPIO\",\"p\":\"LED1\",\"v\":1}\n")` over UART. So every GPIO change is sent as a JSON line.
  - **RX (Simulator / Twin → MCU)**: `platform_adapter.c` has `ISR(USART_RX_vect)`. When it receives a line like `{"t":"GPIO","p":"BUTTON1","v":0}`, it parses and calls `vGpioAVRSetSimulated(acPinName, value)` to inject state (e.g. from Digital Twin or simulator).

So the MCU already **sends** state to the host and **receives** JSON commands to set simulated pin state.

### 1.2 Unified Simulator (Main.py)

- **Owns the serial port** (e.g. COM3, 57600).
- **SerialReader thread**: Reads lines from MCU. If line is JSON with `"t"` in `["WRITE","GPIO"]` and `"p"`, `"v"` → `state_store.update_pin(pin, val)`. So the simulator’s state reflects MCU GPIO.
- **HTTP API**:
  - `GET /api/state` → full state (for Digital Twin or others).
  - `GET /api/gpio/all` → `{"pins": {"LED1": {"value": 1}, ...}}` (what Unity’s SyncEngine pulls).
  - `POST /api/gpio/<pin>` with `{"value": n}` → updates `state_store` only; **does not** currently forward to the MCU.

So today: **MCU → Serial → Simulator (state)** and **Unity → HTTP → Simulator (state)**. Simulator is the bridge; Unity does not push to MCU over serial yet.

### 1.3 Digital Twin (hal_digital_twin_os)

- **HAL_HardwareClient**: `BaseUrl = "http://127.0.0.1:8080"` (unified_simulator).
- **HAL_SyncEngine**: Periodically `GET /api/gpio/all` → `_vProcessInboundState` (updates device model from simulator state). When the user changes something in Unity, `_vProcessOutboundChanges` does `POST /api/gpio/<component_id>` with `{"value": ...}` (updates simulator state only).

So: **Digital Twin sync** = Unity ↔ Simulator over HTTP; **Simulator ↔ MCU** over serial (MCU → simulator today; simulator → MCU possible via the same JSON the MCU already parses in the RX ISR).

---

## 2. What We Change for MCP

### 2.1 MCU side: reuse gpio_driver/examples and implementations; minimal change in gpio_driver

- **Reuse gpio_driver/examples and gpio_driver/implementations**: We do **not** copy platform_adapter or app_main into hal_embedded_mcp. The MCP MCU build uses **the same** `gpio_driver/examples/common/app_main.c` and **the same** `gpio_driver/implementations/avr/platform_adapter.c` (and all other gpio_driver sources). So we still **call** gpio_driver and gpio_driver/implementations as-is.
- **Minimal change in gpio_driver**: Only **one** file in gpio_driver is updated: `implementations/avr/platform_adapter.c`. In the UART RX path, when a full line is received: if it starts with `{` we keep the existing JSON path (`vParseAndApplyInput` for Digital Twin); **else** we call `vMcpHandleLine(acRxBuffer)`. `vMcpHandleLine` is declared **weak** in platform_adapter (no-op stub); when we build the MCP firmware we link `hal_embedded_mcp/mcu/tool_registry.c`, which provides the real `vMcpHandleLine` (registry dispatch). So the default gpio_driver-only build is unchanged; the MCP build just adds our sources and overrides the weak symbol.
- **hal_embedded_mcp only adds**: `tool_registry.c`, `tool_handlers_gpio.c`, generated `mcp_pins_gen.c`, and a CMakeLists (e.g. under `mcu/avr/`) that builds like `gpio_driver/examples/avr` but points to our config and adds the MCP sources. **No** duplicate platform_adapter; **no** change to `examples/common` (app_main stays as-is).
- **Build**: MCP config → `gen_mcp_from_config.py` → `mcp_pins_gen.c`; same config can feed GPIO pin config via gpio_driver’s `gen_config.py`. CMake uses gpio_driver’s app_main and implementation; only extra sources are MCP registry and handlers.

So: **one UART**, two kinds of incoming traffic on the MCU:

1. **JSON line** → existing parser → `vGpioAVRSetSimulated` (Digital Twin / simulator push to MCU).
2. **MCP command line** → registry → `eGpioHelperWrite` / `eGpioHelperRead` → response line.

Outgoing: unchanged JSON lines for state sync + new response lines for MCP.

### 2.2 Simulator side: extract MCP commands and redirect to MCU; digital twin unchanged

- **Keep**: SerialReader still **reads** (RX) from the MCU and updates state from JSON lines; GET/POST for `/api/state`, `/api/gpio/all`, `/api/gpio/<pin>` stay as they are so the **digital twin still works** with no change.
- **Add**: A new endpoint (e.g. `POST /api/mcp/command`) so the simulator can **extract** commands coming from the MCP server (HTTP) and **redirect** them to the MCU over serial (TX):
  - **Request**: e.g. `{"tool":"gpio_write","params":["LED1",0]}` or `{"tool":"gpio_read","params":["LED1"]}`.
  - **Behavior**: Serial port is already open. **Write** the corresponding command line to the MCU (e.g. `gpio_write LED1 0\n`), then **wait for one response line** from the MCU (e.g. `OK` or `GPIO_READ LED1 0`). Return that line (or a small JSON wrapper) as the HTTP response to the MCP server.
  - **Flow**: MCP server → HTTP (POST /api/mcp/command) → simulator **redirects** to MCU (serial TX) → MCU runs handler → MCU sends response line (serial TX) → simulator reads it (serial RX) → simulator returns it to MCP server (HTTP response).

So the simulator **extracts** MCP server commands from HTTP and **redirects** them to the MCU on the same UART; it still processes **RX from the MCU** (state JSON) for the digital twin. **One UART, one process**: digital twin sync (unchanged) + MCP command/response.

### 2.3 Optional: forward Unity POST to MCU

- When the simulator receives `POST /api/gpio/<pin>` with `{"value": n}`, in addition to updating `state_store`, it can **write a JSON line to serial** in the format the MCU already accepts: `{"t":"GPIO","p":"LED1","v":1}\n`. Then the MCU’s existing RX ISR will apply it (`vGpioAVRSetSimulated`). That way Unity can drive the real hardware as well.

---

## 3. Summary

| Piece | Current | After change |
|-------|--------|--------------|
| **MCU** | gpio_driver/examples (app_main, platform adapter, implementations) | **Reproduce** same layout; **minimal main** change: add MCP RX branch (registry, handler, response). Still supports **all gpio_driver/implementations**. Build adds MCP config generator → mcp_pins_gen.c. |
| **Unified simulator** | SerialReader (RX from MCU) → state; HTTP GET/POST for digital twin | Same + **extract** MCP commands from new endpoint and **redirect** to MCU (TX); return MCU response to MCP server. Digital twin **unchanged**. |
| **Digital Twin** | GET/POST to simulator | **Unchanged** (still GET/POST to simulator; simulator still updates state from MCU RX). |
| **MCP server** | (New) | Sends commands via HTTP to simulator; simulator redirects to MCU. No direct serial. |

So: we **reproduce** gpio_driver/examples on the MCU side with **minimal main** change and **update** the unified_simulator to **extract and redirect** RX command from the MCP server to the MCU, so the digital twin still works.
