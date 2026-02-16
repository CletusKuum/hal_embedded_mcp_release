# Why this exists

## The problem

Changing timing or sequences in embedded projects usually means:

- Edit C (state machines, timers, counters)
- Rebuild
- Reflash
- Debug over serial

That cycle slows down **try-and-see** and **demos**. Every behavior change is a firmware change.

## What this does

You keep **one generic firmware**: a small command executor (e.g. `gpio_write`, `gpio_read` over UART). Complex sequences and timing live on the **host** (Python + MCP or inline scripts). You:

- Describe behavior in **natural language** (e.g. “blink 10 times, double the delay each time, repeat 10 rounds”), or
- Run a **script** that sends the same commands over serial with the timing you want.

**No C change, no reflash** for those behavior changes. The same HAL and protocol can target AVR, PC simulation, or STM32.

**Result:** Faster iteration on behavior and demos; less firmware churn.

## Stack

```
  You / AI  →  MCP client (Cursor, etc.)  →  Python server  →  UART  →  MCU (HAL)  →  GPIO / hardware
                   (tools: gpio_write, gpio_read)               serial     (one command at a time)
```

So it’s a **defined path**: MCP ↔ server ↔ serial ↔ MCU ↔ HAL ↔ platform — not “MCU + Python glued together.”

## See also

- [NATURAL_LANGUAGE_EXAMPLES.md](NATURAL_LANGUAGE_EXAMPLES.md) – complex sequences without firmware
- [ARCHITECTURE.md](ARCHITECTURE.md) – config, codegen, data flow
