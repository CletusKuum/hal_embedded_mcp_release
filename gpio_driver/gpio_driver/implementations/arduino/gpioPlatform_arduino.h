//==============================================================================
// GPIO Library - Arduino Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief Arduino GPIO implementation header
//! 
//! This implementation wraps Arduino's digitalWrite/digitalRead APIs for the
//! HAL interface. Platform-specific only — application code remains agnostic.
//------------------------------------------------------------------------------

#ifndef GPIO_PLATFORM_ARDUINO_H
#define GPIO_PLATFORM_ARDUINO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../gpioLib.h"

#ifdef PLATFORM_ARDUINO

// ============================================================================
// External Interface
// ============================================================================

/**
 * @brief Arduino GPIO interface instance
 * 
 * Register this with vHalRegisterGpioInterface() to use Arduino GPIO.
 * Pin mappings come from gpio_config.c (SINGLE SOURCE OF TRUTH).
 * No hardcoded pins here — all configuration is centralized.
 */
extern const sGpioInterface_t sGpioInterfaceArduino;

#endif // PLATFORM_ARDUINO

#ifdef __cplusplus
}
#endif

#endif // GPIO_PLATFORM_ARDUINO_H
