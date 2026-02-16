//==============================================================================
// Logging Library - HTTP Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief HTTP logging implementation header
//------------------------------------------------------------------------------

#ifndef LOG_PLATFORM_HTTP_H
#define LOG_PLATFORM_HTTP_H

#include "../logLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// HTTP Logging Interface
// ============================================================================

/**
 * @brief HTTP logging interface instance
 * 
 * Register this with vHalRegisterLogInterface() to use HTTP output.
 * Uses curl/system commands to send logs via HTTP POST requests.
 */
extern const sLogInterface_t sLogInterfaceHTTP;

#ifdef __cplusplus
}
#endif

#endif // LOG_PLATFORM_HTTP_H
