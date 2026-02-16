//==============================================================================
// Logging Library - File Platform Implementation Header
//------------------------------------------------------------------------------
//! @file
//! @brief File logging implementation header
//------------------------------------------------------------------------------

#ifndef LOG_PLATFORM_FILE_H
#define LOG_PLATFORM_FILE_H

#include "../logLib.h"

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// File Logging Interface
// ============================================================================

/**
 * @brief File logging interface instance
 * 
 * Register this with vHalRegisterLogInterface() to use file output.
 * Uses standard C file I/O (fprintf) for log file output.
 */
extern const sLogInterface_t sLogInterfaceFile;

#ifdef __cplusplus
}
#endif

#endif // LOG_PLATFORM_FILE_H
