//==============================================================================
// GPIO Helper - Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Hybrid implementation: Routes to HTTP (PC) or Serial Bridge (AVR)
//------------------------------------------------------------------------------

#include "gpio_helper.h"
#include "helper_common.h"
#include <stdio.h>
#include <string.h>

// Platform conditional includes
#ifdef PLATFORM_HTTP
// On PC Simulator, we can use the HTTP driver directly
#include "../implementations/pc/gpioLib_http.h"
// External declaration if not in header
extern const sGpioInterface_t sGpioInterfaceHTTP;
#endif

#ifndef PLATFORM_HTTP
// AVR / Hardware Includes
#include "../config/gpio_config.h"
#include "../gpioLib.h"

// Manual declarations to ensure visibility
extern const sGpioPinConfig_t g_psGpioPinConfigs[];
extern bool bGpioAVRGetSimulated(const char *pcPinName);
#endif

// Main Helper Implementation ==================================================

void vGpioHelperInit(void) {
  // Get registered interface
  const sGpioInterface_t *psGpio = psHalGetGpioInterface();
  if (psGpio != NULL && psGpio->vHalGpioInitFunc != NULL) {
    psGpio->vHalGpioInitFunc();
  }
}

eRetType_t eGpioHelperConfigure(const sGpioConfig_t *psConfig) {
  const sGpioInterface_t *psGpio = psHalGetGpioInterface();
  if (psGpio == NULL || psGpio->eHalGpioConfigureFunc == NULL) {
    return RET_TYPE_FAIL;
  }

  return psGpio->eHalGpioConfigureFunc(psConfig);
}

eRetType_t eGpioHelperWrite(const char *pcPinName, bool bValue) {
  eRetType_t eRet = RET_TYPE_FAIL;
  const sGpioInterface_t *psGpio = psHalGetGpioInterface();

  // 1. Execute on Registered Driver (Hardware or Simulator)
  if (psGpio != NULL && psGpio->eHalGpioWriteFunc != NULL) {
    eRet = psGpio->eHalGpioWriteFunc(pcPinName, bValue);
  } else {
    return RET_TYPE_FAIL;
  }

  /* 2. On hardware (AVR/STM32): send MCP response "OK" first so the serial
   *    client gets it, then DT sync JSON. */
  if (eRet == RET_TYPE_SUCCESS) {
    printf("OK\n");
    vHelperSend("GPIO", pcPinName, bValue);
  }

  return eRet;
}

eRetType_t eGpioHelperRead(const char *pcPinName, bool *pbValue) {
  const sGpioInterface_t *psGpio = psHalGetGpioInterface();
  if (psGpio == NULL || psGpio->eHalGpioReadFunc == NULL) {
    return RET_TYPE_FAIL;
  }

  // 1. Get Physical State from HAL
  bool bPhysical = false;
  eRetType_t eRet = psGpio->eHalGpioReadFunc(pcPinName, &bPhysical);
  if (eRet != RET_TYPE_SUCCESS)
    return eRet;

#ifdef PLATFORM_HTTP
  // On PC Simulator, the HAL Read IS the Simulated Read.
  *pbValue = bPhysical;
#else
  // On Hardware (AVR), we need to Merge Physical + Simulated
  bool bSimulated = bGpioAVRGetSimulated(pcPinName);

  // Find Config to know Pull Direction
  // Simple linear search (same as Platform)
  const sGpioPinConfig_t *psConfig = g_psGpioPinConfigs;
  eGpioPull_t ePull = GPIO_PULL_NONE;

  while (psConfig->pcPinName != NULL) {
    if (strcmp(psConfig->pcPinName, pcPinName) == 0) {
      ePull = psConfig->ePull;
      break;
    }
    psConfig++;
  }

  // Merge Logic
  bool bIsPressedPhysical = false;
  if (ePull == GPIO_PULL_UP) {
    // Active Low: 0 = Pressed
    // Result = Physical && Simulated (0 dominates)
    *pbValue = bPhysical && bSimulated;
    bIsPressedPhysical = !bPhysical;
  } else {
    // Active High/None: 1 = Pressed
    // Result = Physical || Simulated (1 dominates)
    *pbValue = bPhysical || bSimulated;
    bIsPressedPhysical = bPhysical;
  }

  // Report to Digital Twin if PHYSICAL caused the Press
  // We send only if Physical is Active (Pressed)
  if (bIsPressedPhysical) {
    // Note: This might spam if called frequently.
    // Ideally we'd detect change, but Helper defines "Current State".
    // We assume App throttles or handles logical edge detection.
    // User requested: "if real press sync with the dt"
    vHelperSend("GPIO", pcPinName, (ePull == GPIO_PULL_UP ? 0 : 1));
  }
#endif

  return RET_TYPE_SUCCESS;
}
