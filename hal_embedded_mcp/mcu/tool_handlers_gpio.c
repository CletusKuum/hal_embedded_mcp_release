//==============================================================================
// HAL Embedded MCP - GPIO Tool Handlers (MCU)
//------------------------------------------------------------------------------
// Called from app_main.c: vMcpHandleLine() parses UART line, looks up registry,
// then calls us with params string (e.g. vHandleGpioWrite("LED1 1")). We
// convert params -> eGpioHelperWrite/eGpioHelperRead; helper auto-calls
// vHelperSend() for Digital Twin sync.
//------------------------------------------------------------------------------

#include "tool_registry.h"
#include "gpio_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Generated pin list for validation (optional) */
extern const char *const g_apcMcpPinNames[];
extern const unsigned int g_u32McpPinCount;

static int bIsValidPin(const char *pcPin) {
    for (unsigned int i = 0; i < g_u32McpPinCount && g_apcMcpPinNames[i] != NULL; i++) {
        if (strcmp(pcPin, g_apcMcpPinNames[i]) == 0)
            return 1;
    }
    return 0;
}

void vHandleGpioWrite(const char *pcParams) {
    char acPin[32];
    int iVal = 0;
    if (sscanf(pcParams, "%31s %d", acPin, &iVal) < 2) {
        printf("ERR gpio_write need PIN VALUE\n");
        return;
    }
    if (!bIsValidPin(acPin)) {
        printf("ERR unknown pin %s\n", acPin);
        return;
    }
    bool bVal = (iVal != 0);
    /* Helper does hardware write and sends "OK" + DT sync on success. */
    eRetType_t eRet = eGpioHelperWrite(acPin, bVal);
    if (eRet != RET_TYPE_SUCCESS)
        printf("ERR %d\n", (int)eRet);
}

void vHandleGpioRead(const char *pcParams) {
    char acPin[32];
    if (sscanf(pcParams, "%31s", acPin) < 1) {
        printf("ERR gpio_read need PIN\n");
        return;
    }
    if (!bIsValidPin(acPin)) {
        printf("ERR unknown pin %s\n", acPin);
        return;
    }
    bool bVal = false;
    /* Same as write: UART params -> eGpioHelperRead(); helper does vHelperSend for DT. */
    eRetType_t eRet = eGpioHelperRead(acPin, &bVal);
    if (eRet == RET_TYPE_SUCCESS)
        printf("GPIO_READ %s %d\n", acPin, bVal ? 1 : 0);
    else
        printf("ERR %d\n", (int)eRet);
}
