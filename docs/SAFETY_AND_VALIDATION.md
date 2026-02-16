# Safety and validation

The MCP server and MCU firmware together act as a **guardrail layer** so the AI cannot send arbitrary or dangerous commands to the hardware.

## What is validated

### Server (Python)

- **Pin names** – `pin_id` must be in the generated list (`MCP_PIN_NAMES`) from `config.json`. Unknown pins get an immediate `ERR unknown pin. Allowed: ...` and are **never** sent over serial.
- **Value type** – `gpio_write` takes a boolean (high/low). The MCP schema and tool implementation do not allow raw integers or register values; the AI cannot “hallucinate” a dangerous voltage or clock setting through this API.
- **No direct register access** – The tools only expose `gpio_write` and `gpio_read`. There is no tool for raw register or memory writes.

### MCU (firmware)

- **Pin validation** – The firmware checks that the pin name is in its generated pin list before calling the HAL. Unknown pins get `ERR unknown pin`.
- **Command parsing** – Only known commands (`gpio_write`, `gpio_read`) are executed. Unknown tools get `ERR unknown tool`.
- **One command at a time** – The protocol is line-based; there is no batch or script injection. Each line is parsed and dispatched once.

## What is not in scope (v1)

- No I2C, SPI, or ADC tools – so the AI cannot directly touch those peripherals.
- No clock or power configuration – no way to request invalid clock speeds or voltages through the MCP tools.
- No flash or EEPROM write tools – no risk of bricking through an AI-suggested tool call.

## Adding new tools or pins

New pins are added via **config** and **codegen**; both server and MCU get the same list. New tools (e.g. a future `read_adc`) would be implemented in the server and firmware with explicit parameter validation (e.g. channel bounds) before any hardware access. The pattern is: validate in the server, validate again in the firmware, then call the HAL.
