//==============================================================================
// Logging Library - AVR Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief AVR logging implementation header using UART/printf
//------------------------------------------------------------------------------

#ifndef LOG_PLATFORM_AVR_H
#define LOG_PLATFORM_AVR_H

#include "../logLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_AVR

// ============================================================================
// AVR Logging Interface
// ============================================================================

/**
 * @brief AVR logging interface instance
 * 
 * Register this with vHalRegisterLogInterface() to use AVR UART output.
 * Uses printf (redirected to UART) for serial output.
 */
extern const sLogInterface_t sLogInterfaceAVR;

#endif // PLATFORM_AVR

#ifdef __cplusplus
}
#endif

#endif // LOG_PLATFORM_AVR_H
