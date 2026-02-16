//==============================================================================
// Logging Library - Arduino Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief Arduino logging implementation header using Serial
//------------------------------------------------------------------------------

#ifndef LOG_PLATFORM_ARDUINO_H
#define LOG_PLATFORM_ARDUINO_H

#include "../logLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_ARDUINO

// ============================================================================
// Arduino Logging Interface
// ============================================================================

/**
 * @brief Arduino logging interface instance
 * 
 * Register this with vHalRegisterLogInterface() to use Arduino Serial output.
 * Uses Serial.print() for Arduino Serial Monitor output.
 */
extern const sLogInterface_t sLogInterfaceArduino;

#endif // PLATFORM_ARDUINO

#ifdef __cplusplus
}
#endif

#endif // LOG_PLATFORM_ARDUINO_H
