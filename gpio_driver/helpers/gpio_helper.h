//==============================================================================
// GPIO Helper - Header
//------------------------------------------------------------------------------
//! @file
//! @brief Helper functions for GPIO with Digital Twin synchronization
//------------------------------------------------------------------------------

#ifndef GPIO_HELPER_H
#define GPIO_HELPER_H

#include "../gpioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// Helper Function Prototypes =================================================

/**
 * @brief Initialize GPIO Helper
 *
 * Calls the underlying HAL initialization.
 */
void vGpioHelperInit(void);

/**
 * @brief Configure a GPIO pin (Helper wrapper)
 *
 * Routes to HAL and optionally synchronizes with Digital Twin.
 */
eRetType_t eGpioHelperConfigure(const sGpioConfig_t *psConfig);

/**
 * @brief Write to a GPIO pin (Helper wrapper)
 *
 * 1. Writes to Real Hardware/Simulator (via HAL)
 * 2. Sends Telemetry Bridge message (if on embedded hardware)
 */
eRetType_t eGpioHelperWrite(const char *pcPinName, bool bValue);

/**
 * @brief Read from a GPIO pin (Helper wrapper)
 */
eRetType_t eGpioHelperRead(const char *pcPinName, bool *pbValue);

#ifdef __cplusplus
}
#endif

#endif // GPIO_HELPER_H
