//==============================================================================
// Logging Library - Console Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief Console logging implementation header (Windows/Linux console)
//------------------------------------------------------------------------------

#ifndef LOG_PLATFORM_CONSOLE_H
#define LOG_PLATFORM_CONSOLE_H

#include "../logLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Console Logging Interface
// ============================================================================

/**
 * @brief Console logging interface instance
 * 
 * Register this with vHalRegisterLogInterface() to use console output.
 * Uses printf/fprintf for Windows/Linux console output.
 */
extern const sLogInterface_t sLogInterfaceConsole;

#ifdef __cplusplus
}
#endif

#endif // LOG_PLATFORM_CONSOLE_H
