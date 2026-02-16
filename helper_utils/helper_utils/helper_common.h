//==============================================================================
// HAL Helper Utils - Common Definitions
//------------------------------------------------------------------------------
//! @file
//! @brief Shared definitions and functions for HAL Helpers (Bridge
//! Communication)
//------------------------------------------------------------------------------

#ifndef HELPER_COMMON_H
#define HELPER_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function Prototypes =========================================================

/**
 * @brief Send a helper/telemetry message to the bridge (via UART/Console)
 *
 * Formats: {"t":"<Cmd>","p":"<Pin>","v":<Val>}
 *
 * @param pcCmd Command type (e.g., "GPIO", "PWM")
 * @param pcPin Pin or Channel name (e.g., "LED1", "MOTOR1")
 * @param bValue Value (1/0, true/false) - extended to int if needed later
 */
void vHelperSend(const char *pcCmd, const char *pcPin, int iValue);

/**
 * @brief Send a helper/telemetry message with STRING value
 * Format: {"t":"<Cmd>","p":"<Pin>","v":"<Val>"}
 */
void vHelperSendString(const char *pcCmd, const char *pcPin,
                       const char *pcValue);

#ifdef __cplusplus
}
#endif

#endif // HELPER_COMMON_H
