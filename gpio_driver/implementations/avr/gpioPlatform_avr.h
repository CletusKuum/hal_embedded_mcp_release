//==============================================================================
// GPIO Library - AVR GPIO Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief AVR-specific GPIO implementation using direct register access
//------------------------------------------------------------------------------

#ifndef GPIO_PLATFORM_AVR_H
#define GPIO_PLATFORM_AVR_H

#include "../../gpioLib.h" // Explicit relative path
#include <stdint.h>

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// AVR GPIO Interface
// ============================================================================
// This interface uses direct AVR register access (DDR, PORT, PIN).
// Configuration is read from config/gpio_config.c during Init().
// ============================================================================

// AVR GPIO Interface structure
extern const sGpioInterface_t sGpioInterfaceAVR;

// Digital Twin Input Injection
void vGpioAVRSetSimulated(const char *pcPinName, bool bValue);

// Digital Twin Input Retrieval
bool bGpioAVRGetSimulated(const char *pcPinName);

#ifdef __cplusplus
}
#endif

#endif // GPIO_PLATFORM_AVR_H
