//==============================================================================
// HAL Embedded MCP - Common application main (manager; all platforms)
//------------------------------------------------------------------------------
//! @file
//! @brief Manager: init only; no hardcoded GPIO. Control is given to MCP
//! through
//!        UART: each received line is parsed and dispatched here into the
//!        corresponding tool handler (e.g. eGpioHelperWrite); helper then
//!        auto-calls vHelperSend() for Digital Twin sync.
//------------------------------------------------------------------------------

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef PLATFORM_AVR
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#endif

#include "gpioLib.h"
#include "helpers/gpio_helper.h"
#include "implementations/logPlatform_console.h"
#include "logLib.h"
#include "tool_registry.h"

#ifdef PLATFORM_AVR
#include "uart_line_callback.h"
#endif

extern const sGpioInterface_t *psGetPlatformGpioInterface(void);
extern void vPlatformDelayMs(uint32_t u32Ms);

#define DELAY_MS(ms) vPlatformDelayMs((uint32_t)(ms))

/* Tool handlers (implemented in tool_handlers_gpio.c); called from registry. */
extern void vHandleGpioWrite(const char *pcParams);
extern void vHandleGpioRead(const char *pcParams);

typedef void (*vToolHandler_t)(const char *pcParams);

typedef struct {
  const char *pcName;
  vToolHandler_t pfHandler;
} sToolEntry_t;

/** Registry: map UART tool name string -> handler. New tools: add entry here
 *  and implement the handler (e.g. in tool_handlers_gpio.c). */
static const sToolEntry_t g_asMcpRegistry[] = {{"gpio_write", vHandleGpioWrite},
                                               {"gpio_read", vHandleGpioRead},
                                               {NULL, NULL}};

/**
 * Where the UART line is "constructed" into a function call.
 * Parse "tool_name param1 param2 ...", lookup tool in g_asMcpRegistry,
 * call the registered handler with the params string. The handler then
 * converts params into the real call (e.g. eGpioHelperWrite("LED1", true))
 * and the helper does vHelperSend() for DT sync.
 */
void vMcpHandleLine(const char *pcLine) {
  if (pcLine == NULL || pcLine[0] == '\0')
    return;

  /* 1) Extract first token = tool name (e.g. "gpio_write") */
  char acTool[32];
  char acParams[96];
  int iToolLen = 0;
  while (pcLine[iToolLen] != '\0' && pcLine[iToolLen] != ' ' && iToolLen < 31)
    iToolLen++;
  if (iToolLen == 0)
    return;
  memcpy(acTool, pcLine, (size_t)iToolLen);
  acTool[iToolLen] = '\0';

  /* 2) Rest of line = params (e.g. "LED1 1") */
  const char *pcRest = pcLine + iToolLen;
  while (*pcRest == ' ')
    pcRest++;
  strncpy(acParams, pcRest, sizeof(acParams) - 1);
  acParams[sizeof(acParams) - 1] = '\0';

  /* 3) Lookup registry and call handler -> eventually eGpioHelperWrite etc. */
  for (size_t i = 0; g_asMcpRegistry[i].pcName != NULL; i++) {
    if (strcmp(acTool, g_asMcpRegistry[i].pcName) == 0) {
      g_asMcpRegistry[i].pfHandler(acParams);
      return;
    }
  }
  printf("ERR unknown tool %s\n", acTool);
}

#ifdef PLATFORM_AVR
/** UART string arrives here. JSON -> DT path; else -> vMcpHandleLine (parse +
 * dispatch above). */
void vOnUartLineReceived(const char *pcLine) {
  if (pcLine == NULL || pcLine[0] == '\0')
    return;
  if (pcLine[0] == '{')
    vApplyReceivedJsonLine(pcLine);
  else
    vMcpHandleLine(pcLine);
}
#endif

bool vAppInit(void) {
  const sGpioInterface_t *psPlatformGpio = psGetPlatformGpioInterface();
  if (psPlatformGpio != NULL)
    vHalRegisterGpioInterface(psPlatformGpio);
  vGpioHelperInit();
  return true;
}

/** Manager loop: dispatch pending UART line (so printf/response runs in main). */
void vAppLoop(void) {
  (void)bUartDispatchPendingLine();
  DELAY_MS(10);
}

int main(void) {
#ifdef PLATFORM_AVR
  MCUSR = 0;
  wdt_disable();
#endif

  if (vAppInit()) {
#ifdef PLATFORM_AVR
    sei(); /* Enable global interrupts for UART RX */
#endif
    while (1) {
      vAppLoop();
    }
  }
  return 0;
}
