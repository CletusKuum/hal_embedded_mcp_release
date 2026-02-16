// ==============================================================================
// Platform Adapter - STM32
// ==============================================================================
// Connects the universal app_main.c to the STM32-specific GPIO implementation
// ==============================================================================

#include "gpioLib.h"
#include "gpioPlatform_stm32.h"
#include <stdint.h>

// Assuming standard HAL or libopencm3 usage
// For this example adapter we assume a hypothetical HAL_Delay is available
// In a real project you might include "main.h" or "stm32f1xx_hal.h"
extern void HAL_Delay(uint32_t Delay);

// ==============================================================================
// Contract Implementation
// ==============================================================================

const sGpioInterface_t *psGetPlatformGpioInterface(void) {
  return &sGpioInterfaceSTM32;
}

void vPlatformDelayMs(uint32_t u32Ms) { HAL_Delay(u32Ms); }

// ==============================================================================
// Helper / Digital Twin Bridge Implementation
// ==============================================================================

#include "helper_common.h"
#include <stdio.h>

// Assuming printf is redirected to UART in syscalls.c or similar
void vHelperSend(const char *pcCmd, const char *pcPin, int iValue) {
  // Format JSON for Serial Bridge
  // Example: {"t":"GPIO","p":"LED1","v":1}
  printf("{\"t\":\"%s\",\"p\":\"%s\",\"v\":%d}\n", pcCmd, pcPin, iValue);
}

void vHelperSendString(const char *pcCmd, const char *pcPin,
                       const char *pcValue) {
  printf("{\"t\":\"%s\",\"p\":\"%s\",\"v\":\"%s\"}\n", pcCmd, pcPin, pcValue);
}
