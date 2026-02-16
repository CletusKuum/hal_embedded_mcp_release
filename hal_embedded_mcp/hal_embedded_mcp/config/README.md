# Config and generated files

Single source of truth: **`config.json`** in this directory.

## Flow (same as gpio_driver pattern)

```
config.json
    │
    ├── gpio_driver/scripts/gen_config.py
    │       → gpio_config_gen.c   (in MCU build dir, e.g. mcu/avr/build/)
    │
    └── hal_embedded_mcp/scripts/gen_mcp_from_config.py
            ├── --c-out       → mcp_pins_gen.c   (in MCU build dir)
            └── --python-out → server/generated/mcp_schema.py
```

## Generated C files used by the MCU

| Generated file        | Script                    | Used by MCU for |
|-----------------------|---------------------------|------------------|
| **gpio_config_gen.c** | gpio_driver `gen_config.py` | **Actual pin control.** Full pin config: `g_psGpioPinConfigs[]` (name, direction, pull, AVR port/pin). Used by GPIO HAL (e.g. gpioPlatform_avr.c) and gpio_helper to drive hardware. |
| **mcp_pins_gen.c**    | hal_embedded_mcp `gen_mcp_from_config.py` | **MCP validation only.** Pin names: `g_apcMcpPinNames[]`, `g_u32McpPinCount`. Used by `tool_handlers_gpio.c` to check that MCP commands use allowed pins. Does **not** control the pin. |

**mcp_pins_gen.c alone is not enough to control a pin.** The MCU can control pins because the same build also generates and links **gpio_config_gen.c** from the same `config.json`. That file provides the real configuration (port, pin, direction, pull) that the HAL uses when `eGpioHelperWrite("LED1", true)` runs. So you need both:

- **gpio_config_gen.c** – same shape as in gpio_driver; defines the hardware pin table (required for control).
- **mcp_pins_gen.c** – MCP-specific; whitelist of pin names for tool param validation only.

Both are produced at build time into the platform build directory (e.g. `mcu/avr/build/`).

## config.json layout

- **pins**: array of `{ name, direction, pull, avr: { port, pin }, ... }` – shared by GPIO and MCP.
- **mcp**: `{ tools: ["gpio_write", "gpio_read"], uart: { baud } }` – used only by MCP (script and server).
