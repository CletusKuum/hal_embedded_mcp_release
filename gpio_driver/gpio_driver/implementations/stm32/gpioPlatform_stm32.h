//==============================================================================
// GPIO Library - STM32 GPIO Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief STM32-specific GPIO implementation using libopencm3
//------------------------------------------------------------------------------

#ifndef GPIO_PLATFORM_STM32_H
#define GPIO_PLATFORM_STM32_H

#include "gpioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// STM32 GPIO Interface
// ============================================================================
// This interface uses libopencm3 for STM32 hardware access.
// Configuration is read from config/gpio_config.c during Init().
// ============================================================================

// STM32 GPIO Interface structure
extern const sGpioInterface_t sGpioInterfaceSTM32;

#ifdef __cplusplus
}
#endif

#endif // GPIO_PLATFORM_STM32_H
