# Natural language vs. firmware: examples

With HAL Embedded MCP you don’t have to write (or flash) new firmware for complex timing and sequences. You describe what you want in natural language; the AI runs it via the MCP server or an inline Python script. The MCU only needs the **generic** firmware (gpio_write / gpio_read over UART). No state machines, no timers, no custom logic in C.

---

## Idea

| In firmware (C on MCU) | With MCP + natural language |
|------------------------|-----------------------------|
| State machine, counters, timer ISRs, “double delay each blink”, “repeat N rounds” | You say: *“Blink LED1 10 times with 2s delay, double the delay after each blink; then repeat that whole pattern 10 times, but double the starting delay each round.”* |
| Hard to tweak: change timing = edit code, rebuild, reflash | You say: *“Do the same but 5 rounds and 1s base.”* — AI adjusts the script and runs it. |

The **complexity lives in the host** (Python, driven by the AI). The MCU stays simple: execute one command at a time.

---

## Example 1: Simple repeat

**You say:**  
*“Blink the LED 5 times with 1 second between each blink.”*

**What happens:**  
The AI runs an inline Python script that opens the serial port, sends `gpio_write LED1 1`, sleeps 1s, sends `gpio_write LED1 0`, sleeps 1s, and repeats 5 times. No firmware change.

---

## Example 2: Doubling delay, then repeat the whole pattern

**You say:**  
*“Blink LED1 10 times with a 2 second delay, but double the delay after each blink. Then repeat that whole pattern 10 times, and each time you repeat, double the starting delay for the round (so first round starts at 2s, second at 4s, third at 8s, and so on).”*

**In firmware you’d need:**  
Nested loops, a “round” counter, a “blink within round” counter, a delay variable that doubles in two places (per-blink and per-round), and careful timer/state design. Easy to get wrong.

**With MCP:**  
The AI writes a short Python script (or one-liner) that:

1. For each of 10 rounds:
   - Base delay = 2 * (2 ** round_index)  (2, 4, 8, 16, … seconds).
   - For each of 10 blinks: turn LED on, wait `delay`, turn LED off, wait `delay`, then double `delay` for the next blink within the round.
2. Sends `gpio_write LED1 1` / `gpio_write LED1 0` over serial and uses `time.sleep()` for timing.

You don’t write or flash any C. You just described the behavior.

**Run it yourself (inline script):**  
See [scripts/example_complex_blink.py](../scripts/example_complex_blink.py). Or ask the AI in a chat: paste the same natural language and it will generate and run the equivalent.

---

## Example 3: Mixed pattern

**You say:**  
*“Blink the LED 3 times fast (0.3s on, 0.3s off), then 3 times slow (1.5s on, 1.5s off). Do that whole thing 4 times.”*

**What happens:**  
The AI runs a script with two inner loops (fast blinks, then slow blinks) inside an outer loop of 4. Again, only serial commands and `time.sleep()`; no firmware change.

---

## How to use this

1. **MCP server running (e.g. in Cursor):**  
   Ask in natural language. The AI may use the **gpio_write** tool for single on/off, or run an **inline Python** script (when the port is free) for sequences.

2. **MCP server not running:**  
   Ask in natural language; the AI will run an inline Python script that opens the serial port and does the sequence. See [CHAT_PROMPT.md](../CHAT_PROMPT.md) (one process per port).

3. **Run the reference script:**  
   `python scripts/example_complex_blink.py` (from `hal_embedded_mcp`; set `HAL_MCP_SERIAL_PORT` / `HAL_MCP_SERIAL_BAUD` if needed). Ensure no other app is using the port.

---

## Takeaway

Complex timing and repetition are **specified in natural language** and **executed on the host**. The MCU stays a simple command executor. That’s the leverage of MCP + HAL Embedded MCP.
