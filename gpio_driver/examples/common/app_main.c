//==============================================================================
// GPIO Library - Universal Example (All Platforms)
//------------------------------------------------------------------------------
//! @file
//! @brief Complete example showing GPIO library usage with structured logging
//!
//! PLATFORM INDEPENDENT - Works on:
//! - Windows (MinGW/MSVC): gcc example_main.c logLib.c -o example.exe
//! - Arduino: avr-gcc with build_arduino.bat
//! - HTTP Simulator: gcc example_main.c logLib.c (select GPIO_SOURCE = HTTP)
//! - STM32: arm-none-eabi-gcc (select GPIO_SOURCE = STM32)
//!
//! Logging: Uses logLib.h for structured JSON output with timestamps,
//! layer information, and error codes (production-ready format)
//!
//! Configuration: Set GPIO pins in gpio_config.c (SINGLE SOURCE OF TRUTH)
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef PLATFORM_AVR
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#endif

// ============================================================================
// Logging Setup (Production-Ready with Structured JSON)
// ============================================================================

#include "logLib.h"

// Choose your logging platform implementation:
// Option 1: Console (Windows/Linux - default)
#include "implementations/logPlatform_console.h"

// Option 2: Arduino Serial (for Arduino platform)
// #include "../../logging_driver/implementations/logPlatform_arduino.h"

// ============================================================================
// Platform Selection & Includes
// ============================================================================

#include "gpioLib.h"
#include "helpers/gpio_helper.h"

// ============================================================================
// Platform Adapter Contract (Implemented in platform_adapter.c)
// ============================================================================
// These functions must be implemented by the platform-specific "glue" code
// linked at build time. This allows this file to remain 100% logic-focused.

extern const sGpioInterface_t *psGetPlatformGpioInterface(void);
extern void vPlatformDelayMs(uint32_t u32Ms);

// Helper macro to map local delay call to platform specific implementation
#define DELAY_MS(ms) vPlatformDelayMs(ms)

// ============================================================================
// Main Function - Platform Independent Application Code
// ============================================================================

// ============================================================================
// Application State (Static for persistence between loop calls)
// ============================================================================
static bool bLedState = false;
static bool bButtonState = false;
static bool bLastButtonState = false;

// ============================================================================
// Application Lifecycle Functions (Arduino/Embedded Friendly)
// ============================================================================

bool vAppInit(void) {
  // MINIMAL initialization to test stability

  // 1. Register GPIO
  const sGpioInterface_t *psPlatformGpio = psGetPlatformGpioInterface();
  if (psPlatformGpio != NULL) {
    vHalRegisterGpioInterface(psPlatformGpio);
  }

  // 2. Initialize Helper/Hardware
  vGpioHelperInit();

  // 3. Initial State Read (without logging)
  // eGpioHelperRead("BUTTON1", &bButtonState);

  return true;
}

void vAppLoop(void) {
  eGpioHelperWrite("LED1", true);
  // eRetType_t eRet = eGpioHelperRead("BUTTON1", &bButtonState);

  /* if (eRet == RET_TYPE_SUCCESS) {
    if (bLastButtonState != bButtonState) {
      // Toggle LED
      bLedState = !bLedState;
      eRet = eGpioHelperWrite("LED1", bLedState);

      if (eRet == RET_TYPE_SUCCESS) {
        LOG_INFO("GPIO_LAYER", RET_TYPE_SUCCESS, "Button %s -> LED %s",
                 bButtonState ? "released" : "pressed",
                 bLedState ? "ON" : "OFF");
      } else {
        LOG_ERROR("GPIO_LAYER", eRet, "Failed to write LED1");
      }
      bLastButtonState = bButtonState;
    }

  } else {
    // Only log errors occasionally to avoid spamming?
    // For now, log every failure as per original behavior.
    LOG_WARNING("GPIO_LAYER", eRet, "Failed to read BUTTON1");
  } */

  // Delay is handled by the caller or platform loop
}

// ============================================================================
// Main Entry Point (PC / STM32 / Standard C)
// ============================================================================

int main(void) {
  // 1. Safety: Disable Watchdog immediately
  MCUSR = 0;
  wdt_disable();

  // 2. Register Platform GPIO Interface (Also inits UART)
  // Note: vAppInit acts as the platform independent entry point

  if (vAppInit()) {
    // 3. Main Loop
    while (1) {
      vAppLoop();
      DELAY_MS(100); // Throttle loop
      eGpioHelperWrite("LED1", false);
      DELAY_MS(100); // Throttle loop
    }
    // } else {
    //   // Initialization failed
    //   while (1) {
    //     eGpioHelperWrite("LED1", true);
    //     DELAY_MS(500);
    //     eGpioHelperWrite("LED1", false);
    //     DELAY_MS(500);
    //   }
  }

  return 0;
}
